#include "napi.h"

using namespace Napi;

Value CreateBoolean(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].As<Boolean>().Value());  
}

Object InitBasicTypesBoolean(Env env) {
  Object exports = Object::New(env);

  exports["createBoolean"] = Function::New(env, CreateBoolean);

  return exports;
}
