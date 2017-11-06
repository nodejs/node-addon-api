#include "napi.h"

using namespace Napi;

Value GetPropertyWithNapiValue(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name key = info[1].As<Name>();
  return obj.Get(static_cast<napi_value>(key));
}

Value GetPropertyWithNapiWrapperValue(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name key = info[1].As<Name>();
  return obj.Get(key);
}

Value GetPropertyWithCStyleString(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String jsKey = info[1].As<String>();
  return obj.Get(jsKey.Utf8Value().c_str());
}

Value GetPropertyWithCppStyleString(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String jsKey = info[1].As<String>();
  return obj.Get(jsKey.Utf8Value());
}
