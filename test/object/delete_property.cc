#include "napi.h"
#include "test_helper.h"

using namespace Napi;

Value DeletePropertyWithUint32(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Number key = info[1].As<Number>();
  return Boolean::New(info.Env(), MaybeUnwrapOr(obj.Delete(key.Uint32Value())));
}

Value DeletePropertyWithNapiValue(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name key = info[1].As<Name>();
  return Boolean::New(info.Env(),
                      MaybeUnwrapOr(obj.Delete(static_cast<napi_value>(key))));
}

Value DeletePropertyWithNapiWrapperValue(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  Name key = info[1].As<Name>();
  return Boolean::New(info.Env(), MaybeUnwrapOr(obj.Delete(key)));
}

Value DeletePropertyWithCStyleString(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String jsKey = info[1].As<String>();
  return Boolean::New(info.Env(),
                      MaybeUnwrapOr(obj.Delete(jsKey.Utf8Value().c_str())));
}

Value DeletePropertyWithCppStyleString(const CallbackInfo& info) {
  Object obj = info[0].As<Object>();
  String jsKey = info[1].As<String>();
  return Boolean::New(info.Env(), MaybeUnwrapOr(obj.Delete(jsKey.Utf8Value())));
}
