#include "napi.h"
#include "test_helper.h"

using namespace Napi;

Value SetPropertyWithNapiValue(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name key = info[1].As<Name>();
  Value value = info[2];
  return Boolean::New(
      info.Env(),
      MaybeUnwrapOr(obj.Set(static_cast<napi_value>(key), value), false));
}

Value SetPropertyWithNapiWrapperValue(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name key = info[1].As<Name>();
  Value value = info[2];
  return Boolean::New(info.Env(), MaybeUnwrapOr(obj.Set(key, value), false));
}

Value SetPropertyWithCStyleString(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String jsKey = info[1].As<String>();
  Value value = info[2];
  return Boolean::New(
      info.Env(),
      MaybeUnwrapOr(obj.Set(jsKey.Utf8Value().c_str(), value), false));
}

Value SetPropertyWithCppStyleString(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String jsKey = info[1].As<String>();
  Value value = info[2];
  return Boolean::New(info.Env(),
                      MaybeUnwrapOr(obj.Set(jsKey.Utf8Value(), value), false));
}
