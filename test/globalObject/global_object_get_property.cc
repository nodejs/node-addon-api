#include "napi.h"

using namespace Napi;

Value GetPropertyWithNapiValueAsKey(const CallbackInfo& info) {
  Object globalObject = info.Env().Global();
  Name key = info[0].As<Name>();
  return globalObject.Get(key);
}

Value GetPropertyWithInt32AsKey(const CallbackInfo& info) {
  Object globalObject = info.Env().Global();
  Number key = info[0].As<Napi::Number>();
  return globalObject.Get(key.Uint32Value());
}

Value GetPropertyWithCStyleStringAsKey(const CallbackInfo& info) {
  Object globalObject = info.Env().Global();
  String cStrkey = info[0].As<String>();
  return globalObject.Get(cStrkey.Utf8Value().c_str());
}

Value GetPropertyWithCppStyleStringAsKey(const CallbackInfo& info) {
  Object globalObject = info.Env().Global();
  String cppStrKey = info[0].As<String>();
  return globalObject.Get(cppStrKey.Utf8Value());
}

void CreateMockTestObject(const CallbackInfo& info) {
  Object globalObject = info.Env().Global();
  Number napi_key = Number::New(info.Env(), 2);
  const char* CStringKey = "c_str_key";

  globalObject.Set(napi_key, "napi_attribute");
  globalObject[CStringKey] = "c_string_attribute";
  globalObject[std::string("cpp_string_key")] = "cpp_string_attribute";
  globalObject[std::string("circular")] = globalObject;
  globalObject[(uint32_t)15] = 15;
}
