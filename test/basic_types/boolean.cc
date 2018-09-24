#include "napi.h"

using namespace Napi;

Value CreateBoolean(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].As<Boolean>().Value());
}

Value CreateEmptyBoolean(const CallbackInfo& info) {
  Boolean* boolean = new Boolean();
  return Boolean::New(info.Env(), boolean->IsEmpty());
}

Value CreateBooleanFromExistingValue(const CallbackInfo& info) {
  Boolean* boolean =  new Boolean(info.Env(), info[0].As<Boolean>());
  return Boolean::New(info.Env(), boolean);
}

Object InitBasicTypesBoolean(Env env) {
  Object exports = Object::New(env);

  exports["createBoolean"] = Function::New(env, CreateBoolean);
  exports["createEmptyBoolean"] = Function::New(env, CreateEmptyBoolean);
  exports["createBooleanFromExistingValue"] = Function::New(env, CreateBooleanFromExistingValue);

  return exports;
}
