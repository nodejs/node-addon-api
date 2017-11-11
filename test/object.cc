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

Array GetPropertyNames(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Array arr = obj.GetPropertyNames();
  return arr;
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
    // VS2013 has lifetime issues when passing temporary objects into the constructor of another
    // object. It generates code to destruct the object as soon as the constructor call returns.
    // Since this isn't a common case for using std::string objects, I'm refactoring the test to
    // work around the issue.
    std::string str1("readonlyAccessor");
    std::string str2("readwriteAccessor");
    std::string str3("readonlyValue");
    std::string str4("readwriteValue");
    std::string str5("enumerableValue");
    std::string str6("configurableValue");
    std::string str7("function");

    obj.DefineProperties({
      PropertyDescriptor::Accessor(str1, TestGetter),
      PropertyDescriptor::Accessor(str2, TestGetter, TestSetter),
      PropertyDescriptor::Value(str3, trueValue),
      PropertyDescriptor::Value(str4, trueValue, napi_writable),
      PropertyDescriptor::Value(str5, trueValue, napi_enumerable),
      PropertyDescriptor::Value(str6, trueValue, napi_configurable),
      PropertyDescriptor::Function(str7, TestFunction),
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

Value DeleteProperty(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name name = info[1].As<Name>();
  return Boolean::New(info.Env(), obj.Delete(name));
}

Value HasOwnProperty(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name name = info[1].As<Name>();
  return Boolean::New(info.Env(), obj.HasOwnProperty(name));
}

Value CreateObjectUsingMagic(const CallbackInfo& info) {
  Env env = info.Env();
  Object obj = Object::New(env);
  obj["cp_false"] = false;
  obj["cp_true"] = true;
  obj[std::string("s_true")] = true;
  obj[std::string("s_false")] = false;
  obj["0"] = 0;
  obj[42] = 120;
  obj["0.0f"] = 0.0f;
  obj["0.0"] = 0.0;
  obj["-1"] = -1;
  obj["foo2"] = u"foo";
  obj[std::string("foo4")] = NAPI_WIDE_TEXT("foo");
  obj[std::string("foo5")] = "foo";
  obj[std::string("foo6")] = std::u16string(NAPI_WIDE_TEXT("foo"));
  obj[std::string("foo7")] = std::string("foo");
  obj[std::string("circular")] = obj;
  obj["circular2"] = obj;
  return obj;
}

Object InitObject(Env env) {
  Object exports = Object::New(env);

  exports["GetPropertyNames"] = Function::New(env, GetPropertyNames);
  exports["defineProperties"] = Function::New(env, DefineProperties);
  exports["defineValueProperty"] = Function::New(env, DefineValueProperty);
  exports["getProperty"] = Function::New(env, GetProperty);
  exports["setProperty"] = Function::New(env, SetProperty);
  exports["deleteProperty"] = Function::New(env, DeleteProperty);
  exports["hasOwnPropertyFromNative"] = Function::New(env, HasOwnProperty);
  exports["createObjectUsingMagic"] = Function::New(env, CreateObjectUsingMagic);

  return exports;
}
