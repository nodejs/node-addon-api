#include "napi.h"

using namespace Napi;

void Freeze(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  obj.Freeze();
}

void Seal(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  obj.Seal();
}

Object InitObjectFreezeSeal(Env env) {
  Object exports = Object::New(env);
  exports["freeze"] = Function::New(env, Freeze);
  exports["seal"] = Function::New(env, Seal);
  return exports;
}
