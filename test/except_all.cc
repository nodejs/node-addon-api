#include <stdexcept>
#include "napi.h"

using namespace Napi;

void ThrowStdException(const CallbackInfo& info) {
  std::string message = info[0].As<String>().Utf8Value();
  throw std::runtime_error(message);
}

void ThrowPrimitiveException(const CallbackInfo&) {
  throw 0;
}

Object Init(Env env, Object exports) {
  exports.Set("throwStdException", Napi::Function::New(env, ThrowStdException));
  exports.Set("throwPrimitiveException",
              Napi::Function::New(env, ThrowPrimitiveException));
  return exports;
}

NODE_API_MODULE(addon, Init)
