#include "napi.h"

using namespace Napi;

void SetPropertyWithNapiValue(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name key = info[1].As<Name>();
  Value value = info[2];
  obj.Set(static_cast<napi_value>(key), value);
}

void SetPropertyWithNapiWrapperValue(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name key = info[1].As<Name>();
  Value value = info[2];
  obj.Set(key, value);
}

void SetPropertyWithCStyleString(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String jsKey = info[1].As<String>();
  Value value = info[2];
  obj.Set(jsKey.Utf8Value().c_str(), value);
}

void SetPropertyWithCppStyleString(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String jsKey = info[1].As<String>();
  Value value = info[2];
  obj.Set(jsKey.Utf8Value(), value);
}
