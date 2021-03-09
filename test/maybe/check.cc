#include "napi.h"
#if defined(NODE_ADDON_API_ENABLE_MAYBE)

using namespace Napi;

namespace {

void VoidCallback(const CallbackInfo& info) {
  Function fn = info[0].As<Function>();

  Maybe<Value> it = fn.Call({});

  it.Check();
}

}  // end anonymous namespace

Object InitMaybeCheck(Env env) {
  Object exports = Object::New(env);
  exports.Set("voidCallback", Function::New(env, VoidCallback));
  return exports;
}
#endif
