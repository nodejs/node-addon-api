#include "napi.h"

#if (NAPI_VERSION > 7)

using namespace Napi;

Value Freeze(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  return Boolean::New(info.Env(), obj.Freeze());
}

Value Seal(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  return Boolean::New(info.Env(), obj.Seal());
}

Object InitObjectFreezeSeal(Env env) {
  Object exports = Object::New(env);
  exports["freeze"] = Function::New(env, Freeze);
  exports["seal"] = Function::New(env, Seal);
  return exports;
}

#endif
