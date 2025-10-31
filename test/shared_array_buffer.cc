#include "napi.h"

using namespace Napi;

namespace {

#ifdef NODE_API_EXPERIMENTAL_HAS_SHAREDARRAYBUFFER
Value TestIsSharedArrayBuffer(const CallbackInfo& info) {
  if (info.Length() < 1) {
    Error::New(info.Env(), "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return Value();
  }

  return Boolean::New(info.Env(), info[0].IsSharedArrayBuffer());
}

Value TestCreateSharedArrayBuffer(const CallbackInfo& info) {
  if (info.Length() < 1) {
    Error::New(info.Env(), "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return Value();
  } else if (!info[0].IsNumber()) {
    Error::New(info.Env(),
               "Wrong type of arguments. Expects a number as first argument.")
        .ThrowAsJavaScriptException();
    return Value();
  }

  auto byte_length = info[0].As<Number>().Uint32Value();
  if (byte_length == 0) {
    Error::New(info.Env(),
               "Invalid byte length. Expects a non-negative integer.")
        .ThrowAsJavaScriptException();
    return Value();
  }

  return SharedArrayBuffer::New(info.Env(), byte_length);
}

Value TestGetSharedArrayBufferInfo(const CallbackInfo& info) {
  if (info.Length() < 1) {
    Error::New(info.Env(), "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return Value();
  } else if (!info[0].IsSharedArrayBuffer()) {
    Error::New(info.Env(),
               "Wrong type of arguments. Expects a SharedArrayBuffer as first "
               "argument.")
        .ThrowAsJavaScriptException();
    return Value();
  }

  auto byte_length = info[0].As<SharedArrayBuffer>().ByteLength();

  return Number::New(info.Env(), byte_length);
}

Value TestSharedArrayBufferData(const CallbackInfo& info) {
  if (info.Length() < 1) {
    Error::New(info.Env(), "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return Value();
  } else if (!info[0].IsSharedArrayBuffer()) {
    Error::New(info.Env(),
               "Wrong type of arguments. Expects a SharedArrayBuffer as first "
               "argument.")
        .ThrowAsJavaScriptException();
    return Value();
  }

  auto byte_length = info[0].As<SharedArrayBuffer>().ByteLength();
  void* data = info[0].As<SharedArrayBuffer>().Data();

  if (byte_length > 0 && data != nullptr) {
    uint8_t* bytes = static_cast<uint8_t*>(data);
    for (size_t i = 0; i < byte_length; i++) {
      bytes[i] = i % 256;
    }

    return Boolean::New(info.Env(), true);
  }

  return Boolean::New(info.Env(), false);
}
#endif
}  // end anonymous namespace

Object InitSharedArrayBuffer(Env env) {
  Object exports = Object::New(env);

#ifdef NODE_API_EXPERIMENTAL_HAS_SHAREDARRAYBUFFER
  exports["testIsSharedArrayBuffer"] =
      Function::New(env, TestIsSharedArrayBuffer);
  exports["testCreateSharedArrayBuffer"] =
      Function::New(env, TestCreateSharedArrayBuffer);
  exports["testGetSharedArrayBufferInfo"] =
      Function::New(env, TestGetSharedArrayBufferInfo);
  exports["testSharedArrayBufferData"] =
      Function::New(env, TestSharedArrayBufferData);
#endif

  return exports;
}
