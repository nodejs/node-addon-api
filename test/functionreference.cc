#include "napi.h"

using namespace Napi;

namespace {
Value Call(const CallbackInfo& info) {
  HandleScope scope(info.Env());
  FunctionReference ref;
  ref.Reset(info[0].As<Function>());

  return ref.Call({});
}

Value Construct(const CallbackInfo& info) {
  HandleScope scope(info.Env());
  FunctionReference ref;
  ref.Reset(info[0].As<Function>());

  return ref.New({});
}
}  // namespace

Object InitFunctionReference(Env env) {
  Object exports = Object::New(env);

  exports["call"] = Function::New(env, Call);
  exports["construct"] = Function::New(env, Construct);

  return exports;
}
