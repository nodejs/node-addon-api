#include "napi.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

using namespace Napi;

Value createScope(const CallbackInfo& info) {
  {
    HandleScope scope(info.Env());
    String::New(info.Env(), "inner-scope");
  }
  return String::New(info.Env(), "scope");
}

Value escapeFromScope(const CallbackInfo& info) {
  Value result;
  {
    EscapableHandleScope scope(info.Env());
    result = scope.Escape(String::New(info.Env(), "inner-scope"));
  }
  return result;
}

#define LOOP_MAX 1000000
Value stressEscapeFromScope(const CallbackInfo& info) {
  Value result;
  for (int i = 0; i < LOOP_MAX; i++) {
    EscapableHandleScope scope(info.Env());
    char buffer[128];
    snprintf(buffer, 128, "%d", i);
    std::string name = std::string("inner-scope") + std::string(buffer);
    Value newValue = String::New(info.Env(), name.c_str());
    if (i == (LOOP_MAX -1)) {
      result = scope.Escape(newValue);
    }
  }
  return result;
}

Value doubleEscapeFromScope(const CallbackInfo& info) {
  Value result;
  {
    EscapableHandleScope scope(info.Env());
    result = scope.Escape(String::New(info.Env(), "inner-scope"));
    result = scope.Escape(String::New(info.Env(), "inner-scope"));
  }
  return result;
}

Object InitHandleScope(Env env) {
  Object exports = Object::New(env);

  exports["createScope"] = Function::New(env, createScope);
  exports["escapeFromScope"] = Function::New(env, escapeFromScope);
  exports["stressEscapeFromScope"] = Function::New(env, stressEscapeFromScope);
  exports["doubleEscapeFromScope"] = Function::New(env, doubleEscapeFromScope);

  return exports;
}
