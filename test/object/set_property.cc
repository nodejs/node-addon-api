#include "napi.h"

using namespace Napi;

Value SetPropertyWithNapiValue(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name key = info[1].As<Name>();
  Value value = info[2];
  return Boolean::New(info.Env(), obj.Set(static_cast<napi_value>(key), value));
}

Value SetPropertyWithNapiWrapperValue(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name key = info[1].As<Name>();
  Value value = info[2];
  return Boolean::New(info.Env(), obj.Set(key, value));
}

Value SetPropertyWithCStyleString(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String jsKey = info[1].As<String>();
  Value value = info[2];
  return Boolean::New(info.Env(), obj.Set(jsKey.Utf8Value().c_str(), value));
}

Value SetPropertyWithCppStyleString(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String jsKey = info[1].As<String>();
  Value value = info[2];
  return Boolean::New(info.Env(), obj.Set(jsKey.Utf8Value(), value));
}
