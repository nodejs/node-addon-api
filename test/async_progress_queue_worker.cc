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

    TestWorker* worker = new TestWorker(
        cb, progress, "TestResource", Object::New(info.Env()), times);

    return Napi::External<TestWorker>::New(info.Env(), worker);
  }

  static void QueueWork(const CallbackInfo& info) {
    auto wrap = info[0].As<Napi::External<TestWorker>>();
    auto worker = wrap.Data();
    worker->Queue();
  }

 protected:
  void Execute(ExecutionProgress& progress) override {
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
    if (!_js_progress_cb.IsEmpty()) {
      Number progress = Number::New(env, data->progress);
      _js_progress_cb.Call(Receiver().Value(), {progress});
    }
  }

 private:
  TestWorker(Function cb,
             Function progress,
             const char* resource_name,
             const Object& resource,
             int32_t times)
      : AsyncProgressQueueWorker(cb, resource_name, resource), _times(times) {
    _js_progress_cb.Reset(progress, 1);
  }

  int32_t _times;
  FunctionReference _js_progress_cb;
};

}  // namespace

Object InitAsyncProgressQueueWorker(Env env) {
  Object exports = Object::New(env);
  exports["createWork"] = Function::New(env, TestWorker::CreateWork);
  exports["queueWork"] = Function::New(env, TestWorker::QueueWork);
  return exports;
}

#endif
