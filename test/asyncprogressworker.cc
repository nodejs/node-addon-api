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

}

Object InitAsyncProgressWorker(Env env) {
  Object exports = Object::New(env);
  exports["doWork"] = Function::New(env, TestWorker::DoWork);
  return exports;
}

#endif
