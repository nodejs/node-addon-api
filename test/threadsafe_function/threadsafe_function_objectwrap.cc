#include <napi.h>
#include <chrono>
#include <thread>

class ThreadSafeFunctionObjectWrap
    : public Napi::ObjectWrap<ThreadSafeFunctionObjectWrap> {
 public:
  static Napi::Value Init(Napi::Env env) {
    return DefineClass(env, "ThreadSafeFunctionObjectWrap", {});
  }

  ThreadSafeFunctionObjectWrap(const Napi::CallbackInfo& info)
      : Napi::ObjectWrap<ThreadSafeFunctionObjectWrap>(info) {
    _tsfn = Napi::ThreadSafeFunction::New(
        info.Env(),
        info[0].As<Napi::Function>(),
        "ThreadSafeFunctionObjectWrap",
        1,
        1,
        this,
        &ThreadSafeFunctionObjectWrap::FinalizerCallback,
        (void*)nullptr);
    _thread =
        std::thread(&ThreadSafeFunctionObjectWrap::Thread, std::ref(_tsfn));
  }

  ~ThreadSafeFunctionObjectWrap() {
    fprintf(stderr, "About to release TSFN\n");
    _tsfn.Release();
    _thread.join();
  }

 private:
  static void FinalizerCallback(Napi::Env,
                                void*,
                                ThreadSafeFunctionObjectWrap*) {
    fprintf(stderr, "TSFN finalizer\n");
  }

  static void Thread(const Napi::ThreadSafeFunction& tsfn) {
    tsfn.Acquire();
    while (true) {
      fprintf(stderr, "Performing blocking call\n");
      napi_status status = tsfn.BlockingCall([](Napi::Env, Napi::Function fn) {
        fprintf(stderr, "Inside blocking call lambda\n");
        fn.Call({});
      });
      fprintf(stderr, "status: %d\n", status);
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

  Napi::ThreadSafeFunction _tsfn;
  std::thread _thread;
};

Napi::Value InitThreadSafeFunctionObjectWrap(Napi::Env env) {
  return ThreadSafeFunctionObjectWrap::Init(env);
}
