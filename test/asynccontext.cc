#include "napi.h"

using namespace Napi;

namespace {

static void MakeCallback(const CallbackInfo& info) {
  Function callback = info[0].As<Function>();
  Object resource = info[1].As<Object>();
  AsyncContext context(info.Env(), "async_context_test", resource);
  callback.MakeCallback(Object::New(info.Env()),
      std::initializer_list<napi_value>{}, context);
}

} // end anonymous namespace

Object InitAsyncContext(Env env) {
  Object exports = Object::New(env);
  exports["makeCallback"] = Function::New(env, MakeCallback);
  return exports;
}
