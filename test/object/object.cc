#include "napi.h"

using namespace Napi;

// Native wrappers for testing Object::Get()
Value GetPropertyWithNapiValue(const CallbackInfo& info);
Value GetPropertyWithNapiWrapperValue(const CallbackInfo& info);
Value GetPropertyWithCStyleString(const CallbackInfo& info);
Value GetPropertyWithCppStyleString(const CallbackInfo& info);

// Native wrappers for testing Object::Set()
void SetPropertyWithNapiValue(const CallbackInfo& info);
void SetPropertyWithNapiWrapperValue(const CallbackInfo& info);
void SetPropertyWithCStyleString(const CallbackInfo& info);
void SetPropertyWithCppStyleString(const CallbackInfo& info);

// Native wrappers for testing Object::Delete()
Value DeletePropertyWithNapiValue(const CallbackInfo& info);
Value DeletePropertyWithNapiWrapperValue(const CallbackInfo& info);
Value DeletePropertyWithCStyleString(const CallbackInfo& info);
Value DeletePropertyWithCppStyleString(const CallbackInfo& info);

// Native wrappers for testing Object::HasOwnProperty()
Value HasOwnPropertyWithNapiValue(const CallbackInfo& info);
Value HasOwnPropertyWithNapiWrapperValue(const CallbackInfo& info);
Value HasOwnPropertyWithCStyleString(const CallbackInfo& info);
Value HasOwnPropertyWithCppStyleString(const CallbackInfo& info);

// Native wrappers for testing Object::Has()
Value HasPropertyWithNapiValue(const CallbackInfo& info);
Value HasPropertyWithNapiWrapperValue(const CallbackInfo& info);
Value HasPropertyWithCStyleString(const CallbackInfo& info);
Value HasPropertyWithCppStyleString(const CallbackInfo& info);

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
  Env env = info.Env();

  Boolean trueValue = Boolean::New(env, true);

  if (nameType.Utf8Value() == "literal") {
    obj.DefineProperties({
#ifndef NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Accessor("readonlyAccessor", TestGetter),
      PropertyDescriptor::Accessor("readwriteAccessor", TestGetter, TestSetter),
#else // NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Accessor(env, obj, "readonlyAccessor", TestGetter),
      PropertyDescriptor::Accessor(env, obj, "readwriteAccessor", TestGetter, TestSetter),
#endif // !NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Value("readonlyValue", trueValue),
      PropertyDescriptor::Value("readwriteValue", trueValue, napi_writable),
      PropertyDescriptor::Value("enumerableValue", trueValue, napi_enumerable),
      PropertyDescriptor::Value("configurableValue", trueValue, napi_configurable),
#ifndef NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Function("function", TestFunction),
#else // NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Function(env, obj, "function", TestFunction),
#endif // !NODE_ADDON_API_DISABLE_DEPRECATED
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
#ifndef NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Accessor(str1, TestGetter),
      PropertyDescriptor::Accessor(str2, TestGetter, TestSetter),
#else // NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Accessor(env, obj, str1, TestGetter),
      PropertyDescriptor::Accessor(env, obj, str2, TestGetter, TestSetter),
#endif // !NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Value(str3, trueValue),
      PropertyDescriptor::Value(str4, trueValue, napi_writable),
      PropertyDescriptor::Value(str5, trueValue, napi_enumerable),
      PropertyDescriptor::Value(str6, trueValue, napi_configurable),
#ifndef NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Function(str7, TestFunction),
#else // NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Function(env, obj, str7, TestFunction),
#endif // !NODE_ADDON_API_DISABLE_DEPRECATED
    });
  } else if (nameType.Utf8Value() == "value") {
    obj.DefineProperties({
#ifndef NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Accessor(
        Napi::String::New(env, "readonlyAccessor"), TestGetter),
      PropertyDescriptor::Accessor(
        Napi::String::New(env, "readwriteAccessor"), TestGetter, TestSetter),
#else // NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Accessor(env, obj,
        Napi::String::New(env, "readonlyAccessor"), TestGetter),
      PropertyDescriptor::Accessor(env, obj,
        Napi::String::New(env, "readwriteAccessor"), TestGetter, TestSetter),
#endif // !NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Value(
        Napi::String::New(env, "readonlyValue"), trueValue),
      PropertyDescriptor::Value(
        Napi::String::New(env, "readwriteValue"), trueValue, napi_writable),
      PropertyDescriptor::Value(
        Napi::String::New(env, "enumerableValue"), trueValue, napi_enumerable),
      PropertyDescriptor::Value(
        Napi::String::New(env, "configurableValue"), trueValue, napi_configurable),
#ifndef NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Function(
        Napi::String::New(env, "function"), TestFunction),
#else // NODE_ADDON_API_DISABLE_DEPRECATED
      PropertyDescriptor::Function(env, obj,
        Napi::String::New(env, "function"), TestFunction),
#endif // !NODE_ADDON_API_DISABLE_DEPRECATED
    });
  }
}

void DefineValueProperty(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name name = info[1].As<Name>();
  Value value = info[2];

  obj.DefineProperty(PropertyDescriptor::Value(name, value));
}

Value CreateObjectUsingMagic(const CallbackInfo& info) {
  Env env = info.Env();
  Object obj = Object::New(env);
  obj["cp_false"] = false;
  obj["cp_true"] = true;
  obj[std::string("s_true")] = true;
  obj[std::string("s_false")] = false;
  obj["0"] = 0;
  obj[(uint32_t)42] = 120;
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

  exports["getPropertyWithNapiValue"] = Function::New(env, GetPropertyWithNapiValue);
  exports["getPropertyWithNapiWrapperValue"] = Function::New(env, GetPropertyWithNapiWrapperValue);
  exports["getPropertyWithCStyleString"] = Function::New(env, GetPropertyWithCStyleString);
  exports["getPropertyWithCppStyleString"] = Function::New(env, GetPropertyWithCppStyleString);

  exports["setPropertyWithNapiValue"] = Function::New(env, SetPropertyWithNapiValue);
  exports["setPropertyWithNapiWrapperValue"] = Function::New(env, SetPropertyWithNapiWrapperValue);
  exports["setPropertyWithCStyleString"] = Function::New(env, SetPropertyWithCStyleString);
  exports["setPropertyWithCppStyleString"] = Function::New(env, SetPropertyWithCppStyleString);

  exports["deletePropertyWithNapiValue"] = Function::New(env, DeletePropertyWithNapiValue);
  exports["deletePropertyWithNapiWrapperValue"] = Function::New(env, DeletePropertyWithNapiWrapperValue);
  exports["deletePropertyWithCStyleString"] = Function::New(env, DeletePropertyWithCStyleString);
  exports["deletePropertyWithCppStyleString"] = Function::New(env, DeletePropertyWithCppStyleString);

  exports["hasOwnPropertyWithNapiValue"] = Function::New(env, HasOwnPropertyWithNapiValue);
  exports["hasOwnPropertyWithNapiWrapperValue"] = Function::New(env, HasOwnPropertyWithNapiWrapperValue);
  exports["hasOwnPropertyWithCStyleString"] = Function::New(env, HasOwnPropertyWithCStyleString);
  exports["hasOwnPropertyWithCppStyleString"] = Function::New(env, HasOwnPropertyWithCppStyleString);

  exports["hasPropertyWithNapiValue"] = Function::New(env, HasPropertyWithNapiValue);
  exports["hasPropertyWithNapiWrapperValue"] = Function::New(env, HasPropertyWithNapiWrapperValue);
  exports["hasPropertyWithCStyleString"] = Function::New(env, HasPropertyWithCStyleString);
  exports["hasPropertyWithCppStyleString"] = Function::New(env, HasPropertyWithCppStyleString);

  exports["createObjectUsingMagic"] = Function::New(env, CreateObjectUsingMagic);

  return exports;
}
