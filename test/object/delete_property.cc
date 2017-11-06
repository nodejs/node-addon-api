#include "napi.h"

using namespace Napi;

Value DeletePropertyWithNapiValue(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name key = info[1].As<Name>();
  return Boolean::New(info.Env(), obj.Delete(static_cast<napi_value>(key)));
}

Value DeletePropertyWithNapiWrapperValue(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name key = info[1].As<Name>();
  return Boolean::New(info.Env(), obj.Delete(key));
}

Value DeletePropertyWithCStyleString(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String jsKey = info[1].As<String>();
  return Boolean::New(info.Env(), obj.Delete(jsKey.Utf8Value().c_str()));
}

Value DeletePropertyWithCppStyleString(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String jsKey = info[1].As<String>();
  return Boolean::New(info.Env(), obj.Delete(jsKey.Utf8Value()));
}
