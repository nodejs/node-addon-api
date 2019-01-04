#define NAPI_EXPERIMENTAL
#include "napi.h"

#if (NAPI_VERSION > 2147483646)
#include <thread>
#include <chrono>

using namespace Napi;

void MultithreadedCallback(const CallbackInfo& info) {
  HandleScope scope(info.Env());

  int32_t thread_count = info[0].ToNumber().Int32Value();
  Function cb = info[1].As<Function>();

  auto&& scheduler = NodeThreadScheduler::Create(info.Env());

  for (int i = 0; i < thread_count; i++) {
    // Reference is not copyable, this is a workaround
    napi_ref ref;
    napi_create_reference(info.Env(), cb, 1, &ref);

    std::thread([scheduler, ref]() {
      using namespace std::chrono_literals;

      // bring in some execution chaos
      std::this_thread::sleep_for(1ns);

      scheduler->RunInNodeThread([ref](Env env) {
        FunctionReference fn_ref(env, ref);
        fn_ref.Call(env.Global(), {});
      });
    }).detach();
  }
}

Object InitNodeThreadScheduler(Env env) {
  Object exports = Object::New(env);

  exports["multithreadedCallback"] = Function::New(env, MultithreadedCallback);
  return exports;
}

#endif // (NAPI_VERSION > 2147483646)