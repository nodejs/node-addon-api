#include "napi.h"

using namespace Napi;

namespace {

void DoNotCatch(const CallbackInfo& info) {
  Function thrower = info[0].As<Function>();
  thrower({});
}

void ThrowApiError(const CallbackInfo& info) {
  // Attempting to call an empty function value will throw an API error.
  Function(info.Env(), nullptr).Call(std::initializer_list<napi_value>{});
}

#ifdef NAPI_CPP_EXCEPTIONS

void ThrowJSError(const CallbackInfo& info) {
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
    return e.Value();
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

void CatchAndRethrowError(const CallbackInfo& info) {
  Function thrower = info[0].As<Function>();
  try {
    thrower({});
  } catch (Error& e) {
    e.Set("caught", Boolean::New(info.Env(), true));
    throw;
  }
}

void ThrowErrorThatEscapesScope(const CallbackInfo& info) {
  HandleScope scope(info.Env());

  std::string message = info[0].As<String>().Utf8Value();
  throw Error::New(info.Env(), message);
}

void CatchAndRethrowErrorThatEscapesScope(const CallbackInfo& info) {
  HandleScope scope(info.Env());
  try {
    ThrowErrorThatEscapesScope(info);
  } catch (Error& e) {
    e.Set("caught", Boolean::New(info.Env(), true));
    throw;
  }
}

#else // NAPI_CPP_EXCEPTIONS

void ThrowJSError(const CallbackInfo& info) {
  std::string message = info[0].As<String>().Utf8Value();
  Error::New(info.Env(), message).ThrowAsJavaScriptException();
}

void ThrowTypeError(const CallbackInfo& info) {
  std::string message = info[0].As<String>().Utf8Value();
  TypeError::New(info.Env(), message).ThrowAsJavaScriptException();
}

void ThrowRangeError(const CallbackInfo& info) {
  std::string message = info[0].As<String>().Utf8Value();
  RangeError::New(info.Env(), message).ThrowAsJavaScriptException();
}

Value CatchError(const CallbackInfo& info) {
  Function thrower = info[0].As<Function>();
  thrower({});

  Env env = info.Env();
  if (env.IsExceptionPending()) {
    Error e = env.GetAndClearPendingException();
    return e.Value();
  }
  return info.Env().Null();
}

Value CatchErrorMessage(const CallbackInfo& info) {
  Function thrower = info[0].As<Function>();
  thrower({});

  Env env = info.Env();
  if (env.IsExceptionPending()) {
    Error e = env.GetAndClearPendingException();
    std::string message = e.Message();
    return String::New(env, message);
  }
  return info.Env().Null();
}

void CatchAndRethrowError(const CallbackInfo& info) {
  Function thrower = info[0].As<Function>();
  thrower({});

  Env env = info.Env();
  if (env.IsExceptionPending()) {
    Error e = env.GetAndClearPendingException();
    e.Set("caught", Boolean::New(info.Env(), true));
    e.ThrowAsJavaScriptException();
  }
}

void ThrowErrorThatEscapesScope(const CallbackInfo& info) {
  HandleScope scope(info.Env());

  std::string message = info[0].As<String>().Utf8Value();
  Error::New(info.Env(), message).ThrowAsJavaScriptException();
}

void CatchAndRethrowErrorThatEscapesScope(const CallbackInfo& info) {
  HandleScope scope(info.Env());
  ThrowErrorThatEscapesScope(info);

  Env env = info.Env();
  if (env.IsExceptionPending()) {
    Error e = env.GetAndClearPendingException();
    e.Set("caught", Boolean::New(info.Env(), true));
    e.ThrowAsJavaScriptException();
  }
}

#endif // NAPI_CPP_EXCEPTIONS

void ThrowFatalError(const CallbackInfo& info) {
  Error::Fatal("Error::ThrowFatalError", "This is a fatal error");
}

} // end anonymous namespace

Object InitError(Env env) {
  Object exports = Object::New(env);
  exports["throwApiError"] = Function::New(env, ThrowApiError);
  exports["throwJSError"] = Function::New(env, ThrowJSError);
  exports["throwTypeError"] = Function::New(env, ThrowTypeError);
  exports["throwRangeError"] = Function::New(env, ThrowRangeError);
  exports["catchError"] = Function::New(env, CatchError);
  exports["catchErrorMessage"] = Function::New(env, CatchErrorMessage);
  exports["doNotCatch"] = Function::New(env, DoNotCatch);
  exports["catchAndRethrowError"] = Function::New(env, CatchAndRethrowError);
  exports["throwErrorThatEscapesScope"] = Function::New(env, ThrowErrorThatEscapesScope);
  exports["catchAndRethrowErrorThatEscapesScope"] =
    Function::New(env, CatchAndRethrowErrorThatEscapesScope);
  exports["throwFatalError"] = Function::New(env, ThrowFatalError);
  return exports;
}
