#include "napi.h"

using namespace Napi;

namespace {

int testData = 1;

// Helpers for testing non-Javascript values.
Value CreateExternal(const CallbackInfo& info) {
  return External<int>::New(info.Env(), &testData);
}

} // end anonymous namespace

static Value IsEmpty(const CallbackInfo& info) {
  Value value;
  return Boolean::New(info.Env(), value.IsEmpty());
}

static Value IsUndefined(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsUndefined());
}

static Value IsNull(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsNull());
}

static Value IsBoolean(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsBoolean());
}

static Value IsNumber(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsNumber());
}

static Value IsString(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsString());
}

static Value IsSymbol(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsSymbol());
}

static Value IsArray(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsArray());
}

static Value IsArrayBuffer(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsArrayBuffer());
}

static Value IsTypedArray(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsTypedArray());
}

static Value IsObject(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsObject());
}

static Value IsFunction(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsFunction());
}

static Value IsPromise(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsPromise());
}

static Value IsDataView(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsDataView());
}

static Value IsExternal(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsExternal());
}

static Value ToBoolean(const CallbackInfo& info) {
  return info[0].ToBoolean();
}

static Value ToNumber(const CallbackInfo& info) {
  return info[0].ToNumber();
}

static Value ToString(const CallbackInfo& info) {
  return info[0].ToString();
}

static Value ToObject(const CallbackInfo& info) {
  return info[0].ToObject();
}

Object InitBasicTypesValue(Env env) {
  Object exports = Object::New(env);

  exports["isEmpty"] = Function::New(env, IsEmpty);
  exports["isUndefined"] = Function::New(env, IsUndefined);
  exports["isNull"] = Function::New(env, IsNull);
  exports["isBoolean"] = Function::New(env, IsBoolean);
  exports["isNumber"] = Function::New(env, IsNumber);
  exports["isString"] = Function::New(env, IsString);
  exports["isSymbol"] = Function::New(env, IsSymbol);
  exports["isArray"] = Function::New(env, IsArray);
  exports["isArrayBuffer"] = Function::New(env, IsArrayBuffer);
  exports["isTypedArray"] = Function::New(env, IsTypedArray);
  exports["isObject"] = Function::New(env, IsObject);
  exports["isFunction"] = Function::New(env, IsFunction);
  exports["isPromise"] = Function::New(env, IsPromise);
  exports["isDataView"] = Function::New(env, IsDataView);
  exports["isExternal"] = Function::New(env, IsExternal);
  exports["toBoolean"] = Function::New(env, ToBoolean);
  exports["toNumber"] = Function::New(env, ToNumber);
  exports["toString"] = Function::New(env, ToString);
  exports["toObject"] = Function::New(env, ToObject);

  exports["createExternal"] = Function::New(env, CreateExternal);

  return exports;
}
