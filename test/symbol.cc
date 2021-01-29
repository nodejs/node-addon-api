#include "napi.h"

using namespace Napi;

Value InitializeSymbol(const CallbackInfo& info) {
  String input = info[0].As<String>();
  if (input.IsUndefined()) {
    return Symbol::New(info.Env());
  }
  return Symbol::New(info.Env(), input);
}

Value IsSymbol(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].Type() == napi_symbol);
}

Object InitSymbol(Env env) {
  Object exports = Object::New(env);
  exports["InitializeSymbol"] = Function::New(env, InitializeSymbol);
  exports["IsSymbol"] = Function::New(env, IsSymbol);
  return exports;
}
