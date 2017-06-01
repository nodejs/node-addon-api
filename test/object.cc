#include "napi.h"

using namespace Napi;

static bool testValue = true;

Value TestGetter(const CallbackInfo& info) {
   return Boolean::New(info.Env(), testValue);
}

void TestSetter(const CallbackInfo& info) {
   testValue = info[0].As<Boolean>();
}

Value TestFunction(const CallbackInfo& info) {
   return Boolean::New(info.Env(), true);
}

void DefineProperties(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String nameType = info[1].As<String>();

  Boolean trueValue = Boolean::New(info.Env(), true);

  if (nameType.Utf8Value() == "literal") {
    obj.DefineProperties({
      PropertyDescriptor::Accessor("readonlyAccessor", TestGetter),
      PropertyDescriptor::Accessor("readwriteAccessor", TestGetter, TestSetter),
      PropertyDescriptor::Value("readonlyValue", trueValue),
      PropertyDescriptor::Value("readwriteValue", trueValue, napi_writable),
      PropertyDescriptor::Value("enumerableValue", trueValue, napi_enumerable),
      PropertyDescriptor::Value("configurableValue", trueValue, napi_configurable),
      PropertyDescriptor::Function("function", TestFunction),
    });
  } else if (nameType.Utf8Value() == "string") {
    obj.DefineProperties({
      PropertyDescriptor::Accessor(std::string("readonlyAccessor"), TestGetter),
      PropertyDescriptor::Accessor(std::string("readwriteAccessor"), TestGetter, TestSetter),
      PropertyDescriptor::Value(std::string("readonlyValue"), trueValue),
      PropertyDescriptor::Value(std::string("readwriteValue"), trueValue, napi_writable),
      PropertyDescriptor::Value(std::string("enumerableValue"), trueValue, napi_enumerable),
      PropertyDescriptor::Value(std::string("configurableValue"), trueValue, napi_configurable),
      PropertyDescriptor::Function(std::string("function"), TestFunction),
    });
  } else if (nameType.Utf8Value() == "value") {
    obj.DefineProperties({
      PropertyDescriptor::Accessor(
        Napi::String::New(info.Env(), "readonlyAccessor"), TestGetter),
      PropertyDescriptor::Accessor(
        Napi::String::New(info.Env(), "readwriteAccessor"), TestGetter, TestSetter),
      PropertyDescriptor::Value(
        Napi::String::New(info.Env(), "readonlyValue"), trueValue),
      PropertyDescriptor::Value(
        Napi::String::New(info.Env(), "readwriteValue"), trueValue, napi_writable),
      PropertyDescriptor::Value(
        Napi::String::New(info.Env(), "enumerableValue"), trueValue, napi_enumerable),
      PropertyDescriptor::Value(
        Napi::String::New(info.Env(), "configurableValue"), trueValue, napi_configurable),
      PropertyDescriptor::Function(
        Napi::String::New(info.Env(), "function"), TestFunction),
    });
  }
}

void DefineValueProperty(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name name = info[1].As<Name>();
  Value value = info[2];

  obj.DefineProperty(PropertyDescriptor::Value(name, value));
}

Value GetProperty(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name name = info[1].As<Name>();
  Value value = obj.Get(name);
  return value;
}

void SetProperty(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name name = info[1].As<Name>();
  Value value = info[2];
  obj.Set(name, value);
}

Object InitObject(Env env) {
  Object exports = Object::New(env);

  exports["defineProperties"] = Function::New(env, DefineProperties);
  exports["defineValueProperty"] = Function::New(env, DefineValueProperty);
  exports["getProperty"] = Function::New(env, GetProperty);
  exports["setProperty"] = Function::New(env, SetProperty);

  return exports;
}
