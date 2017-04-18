#include "napi.h"

using namespace Napi;

void ThrowError(const CallbackInfo& info) {
  std::string message = info[0].As<String>().Utf8Value();
  throw Error::New(info.Env(), message);
}

void ThrowTypeError(const CallbackInfo& info) {
  std::string message = info[0].As<String>().Utf8Value();
  throw TypeError::New(info.Env(), message);
}

void ThrowRangeError(const CallbackInfo& info) {
  std::string message = info[0].As<String>().Utf8Value();
  throw RangeError::New(info.Env(), message);
}

Value CatchError(const CallbackInfo& info) {
  Function thrower = info[0].As<Function>();
  try {
    thrower({});
  } catch (const Error& e) {
     return e;
  }
  return info.Env().Null();
}

Value CatchErrorMessage(const CallbackInfo& info) {
  Function thrower = info[0].As<Function>();
  try {
    thrower({});
  } catch (const Error& e) {
     std::string message = e.Message();
     return String::New(info.Env(), message);
  }
  return info.Env().Null();
}

void DoNotCatch(const CallbackInfo& info) {
  Function thrower = info[0].As<Function>();
  thrower({});
}

void CatchAndRethrowError(const CallbackInfo& info) {
  Function thrower = info[0].As<Function>();
  try {
    thrower({});
  } catch (Error& e) {
     e["caught"] = Boolean::New(info.Env(), true);
     throw e;
  }
}

Object InitError(Env env) {
  Object exports = Object::New(env);
  exports["throwError"] = Function::New(env, ThrowError);
  exports["throwTypeError"] = Function::New(env, ThrowTypeError);
  exports["throwRangeError"] = Function::New(env, ThrowRangeError);
  exports["catchError"] = Function::New(env, CatchError);
  exports["catchErrorMessage"] = Function::New(env, CatchErrorMessage);
  exports["doNotCatch"] = Function::New(env, DoNotCatch);
  exports["catchAndRethrowError"] = Function::New(env, CatchAndRethrowError);
  return exports;
}
