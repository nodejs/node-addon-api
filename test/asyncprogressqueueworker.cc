#include "napi.h"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace {

struct ProgressData {
  int32_t progress;
};

class TestWorker : public AsyncProgressQueueWorker<ProgressData> {
public:
  static Napi::Value CreateWork(const CallbackInfo& info) {
    int32_t times = info[0].As<Number>().Int32Value();
    Function cb = info[1].As<Function>();
    Function progress = info[2].As<Function>();

    TestWorker* worker = new TestWorker(cb, progress, "TestResource", Object::New(info.Env()));
    worker->_times = times;

    return Napi::External<TestWorker>::New(info.Env(), worker);
  }

  static Napi::Value QueueWork(const CallbackInfo& info) {
    auto wrap = info[0].As<Napi::External<TestWorker>>();
    auto worker = wrap.Data();
    worker->Queue();
    return Napi::Boolean::New(info.Env(), true);
  }

  static Napi::Value CancelWork(const CallbackInfo& info) {
    auto wrap = info[0].As<Napi::External<TestWorker>>();
    auto worker = wrap.Data();
    // We cannot cancel a worker if it got started. So we have to do a quick cancel.
    worker->Queue();
    worker->Cancel();
    return Napi::Boolean::New(info.Env(), true);
  }

protected:
  void Execute(const ExecutionProgress& progress) override {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);

    if (_times < 0) {
      SetError("test error");
    }
    ProgressData data{0};
    for (int32_t idx = 0; idx < _times; idx++) {
      data.progress = idx;
      progress.Send(&data, 1);
    }
  }

  void OnProgress(const ProgressData* data, size_t /* count */) override {
    Napi::Env env = Env();
    if (!_progress.IsEmpty()) {
      Number progress = Number::New(env, data->progress);
      _progress.MakeCallback(Receiver().Value(), { progress });
    }
  }

private:
  TestWorker(Function cb, Function progress, const char* resource_name, const Object& resource)
    : AsyncProgressQueueWorker(cb, resource_name, resource) {
    _progress.Reset(progress, 1);
  }

  int32_t _times;
  FunctionReference _progress;
};

} // namespace

Object InitAsyncProgressQueueWorker(Env env) {
  Object exports = Object::New(env);
  exports["createWork"] = Function::New(env, TestWorker::CreateWork);
  exports["queueWork"] = Function::New(env, TestWorker::QueueWork);
  exports["cancelWork"] = Function::New(env, TestWorker::CancelWork);
  return exports;
}

#endif
