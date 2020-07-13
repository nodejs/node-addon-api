#include "napi.h"

using namespace Napi;

static Reference<Buffer<uint8_t>> weak;

void  CreateWeakArray(const CallbackInfo& info) {
  weak = Weak(Buffer<uint8_t>::New(info.Env(), 1));
  weak.SuppressDestruct();
}

napi_value AccessWeakArrayEmpty(const CallbackInfo& info) {
  Buffer<uint8_t> value = weak.Value();
  return Napi::Boolean::New(info.Env(), value.IsEmpty());
}

Object InitReference(Env env) {
  Object exports = Object::New(env);

  exports["createWeakArray"] = Function::New(env, CreateWeakArray);
  exports["accessWeakArrayEmpty"] = Function::New(env, AccessWeakArrayEmpty);

  return exports;
}
