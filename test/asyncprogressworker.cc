#include "napi.h"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace {

struct ProgressData {
  size_t progress;
};

class TestWorker : public AsyncProgressWorker<ProgressData> {
public:
  static void DoWork(const CallbackInfo& info) {
    int32_t times = info[0].As<Number>().Int32Value();
    Function cb = info[1].As<Function>();
    Function progress = info[2].As<Function>();

    TestWorker* worker = new TestWorker(cb, progress, "TestResource", Object::New(info.Env()));
    worker->_times = times;
    worker->Queue();
  }

protected:
  void Execute(const ExecutionProgress& progress) override {
    if (_times < 0) {
      SetError("test error");
    }
    ProgressData data{0};
    std::unique_lock<std::mutex> lock(_cvm);
    for (int32_t idx = 0; idx < _times; idx++) {
      data.progress = idx;
      progress.Send(&data, 1);
      _cv.wait(lock);
    }
  }

  void OnProgress(const ProgressData* data, size_t /* count */) override {
    Napi::Env env = Env();
    if (!_progress.IsEmpty()) {
      Number progress = Number::New(env, data->progress);
      _progress.MakeCallback(Receiver().Value(), { progress });
    }
    _cv.notify_one();
  }

private:
  TestWorker(Function cb, Function progress, const char* resource_name, const Object& resource)
      : AsyncProgressWorker(cb, resource_name, resource) {
    _progress.Reset(progress, 1);
  }
  std::condition_variable _cv;
  std::mutex _cvm;
  int32_t _times;
  FunctionReference _progress;
};

class MalignWorker : public AsyncProgressWorker<ProgressData> {
 public:
  static void DoWork(const CallbackInfo& info) {
    Function cb = info[0].As<Function>();
    Function progress = info[1].As<Function>();

    MalignWorker* worker =
        new MalignWorker(cb, progress, "TestResource", Object::New(info.Env()));
    worker->Queue();
  }

 protected:
  void Execute(const ExecutionProgress& progress) override {
    std::unique_lock<std::mutex> lock(_cvm);
    // Testing a nullptr send is acceptable.
    progress.Send(nullptr, 0);
    _cv.wait(lock);
    // Testing busy looping on send doesn't trigger unexpected empty data
    // OnProgress call.
    for (size_t i = 0; i < 1000000; i++) {
      ProgressData data{0};
      progress.Send(&data, 1);
    }
    _cv.wait(lock);
  }

  void OnProgress(const ProgressData* data, size_t count) override {
    Napi::Env env = Env();
    _test_case_count++;
    bool error = false;
    Napi::String reason = Napi::String::New(env, "No error");
    if (_test_case_count == 1 && count != 0) {
      error = true;
      reason = Napi::String::New(env, "expect 0 count of data on 1st call");
    }
    if (_test_case_count > 1 && count != 1) {
      error = true;
      reason = Napi::String::New(env, "expect 1 count of data on non-1st call");
    }
    _progress.MakeCallback(Receiver().Value(),
                           {Napi::Boolean::New(env, error), reason});
    _cv.notify_one();
  }

 private:
  MalignWorker(Function cb,
               Function progress,
               const char* resource_name,
               const Object& resource)
      : AsyncProgressWorker(cb, resource_name, resource) {
    _progress.Reset(progress, 1);
  }

  size_t _test_case_count = 0;
  std::condition_variable _cv;
  std::mutex _cvm;
  FunctionReference _progress;
};
}

Object InitAsyncProgressWorker(Env env) {
  Object exports = Object::New(env);
  exports["doWork"] = Function::New(env, TestWorker::DoWork);
  exports["doMalignTest"] = Function::New(env, MalignWorker::DoWork);
  return exports;
}

#endif
