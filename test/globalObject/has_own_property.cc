#include "napi.h"

using namespace Napi;

Value HasPropertyWithCStyleStringAsKey(const CallbackInfo& info) {
  Object globalObject = info.Env().Global();
  String key = info[0].As<String>();
  return Boolean::New(info.Env(),
                      globalObject.HasOwnProperty(key.Utf8Value().c_str()));
}

Value HasPropertyWithCppStyleStringAsKey(const CallbackInfo& info) {
  Object globalObject = info.Env().Global();
  String key = info[0].As<String>();
  return Boolean::New(info.Env(), globalObject.HasOwnProperty(key.Utf8Value()));
}

Value HasPropertyWithInt32AsKey(const CallbackInfo& info) {
  Object globalObject = info.Env().Global();
  Number key = info[0].As<Number>();
  return Boolean::New(info.Env(), globalObject.HasOwnProperty(key));
}

Value HasPropertyWithNapiValueAsKey(const CallbackInfo& info) {
  Object globalObject = info.Env().Global();
  Name key = info[0].As<Name>();
  return Boolean::New(
      info.Env(), globalObject.HasOwnProperty(static_cast<napi_value>(key)));
}