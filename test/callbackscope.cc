#include "napi.h"

using namespace Napi;

#if (NAPI_VERSION > 2)
namespace {

static void RunInCallbackScope(const CallbackInfo& info) {
  Function callback = info[0].As<Function>();
  AsyncContext context(info.Env(), "callback_scope_test");
  CallbackScope scope(info.Env(), context);
  callback.Call({});
}

} // end anonymous namespace

Object InitCallbackScope(Env env) {
  Object exports = Object::New(env);
  exports["runInCallbackScope"] = Function::New(env, RunInCallbackScope);
  return exports;
}
#endif
