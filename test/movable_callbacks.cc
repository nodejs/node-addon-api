#include <iostream>
#include "napi.h"

using namespace Napi;

Value createExternal(const CallbackInfo& info) {
  FunctionReference ref = Reference<Function>::New(info[0].As<Function>(), 1);

#ifdef NODE_API_EXPERIMENTAL_HAS_POST_FINALIZER
  auto env = info.Env();
  auto ret = External<char>::New(env, nullptr);

  env.AddPostFinalizer(
      [ref = std::move(ref)](Napi::Env /*env*/) { ref.Call({}); });
#else
  auto ret = External<char>::New(
      info.Env(),
      nullptr,
      [ref = std::move(ref)](Napi::Env /*env*/, char* /*data*/) {
        ref.Call({});
      });
#endif
  return ret;
}

Object InitMovableCallbacks(Env env) {
  Object exports = Object::New(env);

  exports["createExternal"] = Function::New(env, createExternal);

  return exports;
}
