#include "napi.h"

using namespace Napi;

Value HasOwnPropertyWithNapiValue(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name key = info[1].As<Name>();
  return Boolean::New(info.Env(), obj.HasOwnProperty(static_cast<napi_value>(key)));
}

Value HasOwnPropertyWithNapiWrapperValue(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name key = info[1].As<Name>();
  return Boolean::New(info.Env(), obj.HasOwnProperty(key));
}

Value HasOwnPropertyWithCStyleString(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String jsKey = info[1].As<String>();
  return Boolean::New(info.Env(), obj.HasOwnProperty(jsKey.Utf8Value().c_str()));
}

Value HasOwnPropertyWithCppStyleString(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String jsKey = info[1].As<String>();
  return Boolean::New(info.Env(), obj.HasOwnProperty(jsKey.Utf8Value()));
}
