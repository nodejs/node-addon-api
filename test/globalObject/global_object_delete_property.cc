#include "napi.h"
#include "test_helper.h"

using namespace Napi;

Value DeletePropertyWithCStyleStringAsKey(const CallbackInfo& info) {
  Object globalObject = info.Env().Global();
  String key = info[0].UnsafeAs<String>();
  return Boolean::New(
      info.Env(), MaybeUnwrap(globalObject.Delete(key.Utf8Value().c_str())));
}

Value DeletePropertyWithCppStyleStringAsKey(const CallbackInfo& info) {
  Object globalObject = info.Env().Global();
  String key = info[0].UnsafeAs<String>();
  return Boolean::New(info.Env(),
                      MaybeUnwrap(globalObject.Delete(key.Utf8Value())));
}

Value DeletePropertyWithInt32AsKey(const CallbackInfo& info) {
  Object globalObject = info.Env().Global();
  Number key = info[0].UnsafeAs<Number>();
  return Boolean::New(info.Env(),
                      MaybeUnwrap(globalObject.Delete(key.Uint32Value())));
}

Value DeletePropertyWithNapiValueAsKey(const CallbackInfo& info) {
  Object globalObject = info.Env().Global();
  Name key = info[0].UnsafeAs<Name>();
  return Boolean::New(info.Env(), MaybeUnwrap(globalObject.Delete(key)));
}
