#include "napi.h"
#include "test_helper.h"

using namespace Napi;

namespace {

Value ConstructRefFromExisitingRef(const CallbackInfo& info) {
  HandleScope scope(info.Env());
  FunctionReference ref;
  FunctionReference movedRef;
  ref.Reset(info[0].As<Function>());
  movedRef = std::move(ref);

  return movedRef({});
}

Value CallWithVectorArgs(const CallbackInfo& info) {
  HandleScope scope(info.Env());
  std::vector<napi_value> newVec;
  FunctionReference ref;
  ref.Reset(info[0].As<Function>());
  newVec.reserve(info.Length() - 1);
  for (int i = 1; i < (int)info.Length(); i++) {
    newVec.push_back(info[i]);
  }
  return ref.Call(newVec);
}

Value CallWithInitList(const CallbackInfo& info) {
  HandleScope scope(info.Env());
  FunctionReference ref;
  ref.Reset(info[0].As<Function>());

  return ref.Call({info[1], info[2], info[3]});
}

Value CallWithRecvInitList(const CallbackInfo& info) {
  HandleScope scope(info.Env());
  FunctionReference ref;
  ref.Reset(info[0].As<Function>());

  return ref.Call(info[1], {info[2], info[3], info[4]});
}

Value CallWithRecvVector(const CallbackInfo& info) {
  HandleScope scope(info.Env());
  FunctionReference ref;
  std::vector<napi_value> newVec;
  ref.Reset(info[0].As<Function>());
  newVec.reserve(info.Length() - 1);
  for (int i = 2; i < (int)info.Length(); i++) {
    newVec.push_back(info[i]);
  }
  return ref.Call(info[1], newVec);
}

Value CallWithRecvArgc(const CallbackInfo& info) {
  HandleScope scope(info.Env());
  FunctionReference ref;
  int argLength = info.Length() - 2;

  napi_value* args = new napi_value[argLength];
  ref.Reset(info[0].As<Function>());
  int argIdx = 0;
  for (int i = 2; i < (int)info.Length(); i++, argIdx++) {
    args[argIdx] = info[i];
  }

  return ref.Call(info[1], argLength, args);
}

Value MakeAsyncCallbackWithInitList(const Napi::CallbackInfo& info) {
  Napi::FunctionReference ref;
  ref.Reset(info[0].As<Function>());

  Napi::AsyncContext contxt(info.Env(), "func_ref_resources", {});

  return ref.MakeCallback(Napi::Object::New(info.Env()), {}, contxt);
}

Value MakeAsyncCallbackWithVector(const Napi::CallbackInfo& info) {
  Napi::FunctionReference ref;
  ref.Reset(info[0].As<Function>());
  std::vector<napi_value> newVec;
  Napi::AsyncContext contxt(info.Env(), "func_ref_resources", {});
  for (int i = 1; i < (int)info.Length(); i++) {
    newVec.push_back(info[i]);
  }
  return ref.MakeCallback(Napi::Object::New(info.Env()), newVec, contxt);
}

Value MakeAsyncCallbackWithArgv(const Napi::CallbackInfo& info) {
  Napi::FunctionReference ref;
  ref.Reset(info[0].As<Function>());
  int argLength = info.Length() - 1;
  napi_value* args = new napi_value[argLength];
  int argIdx = 0;
  for (int i = 1; i < (int)info.Length(); i++, argIdx++) {
    args[argIdx] = info[i];
  }
  Napi::AsyncContext contxt(info.Env(), "func_ref_resources", {});
  return ref.MakeCallback(
      Napi::Object::New(info.Env()), argLength, args, contxt);
}

Value Call(const CallbackInfo& info) {
  HandleScope scope(info.Env());
  FunctionReference ref;
  ref.Reset(info[0].As<Function>());

  return MaybeUnwrapOr(ref.Call({}), Value());
}

Value Construct(const CallbackInfo& info) {
  HandleScope scope(info.Env());
  FunctionReference ref;
  ref.Reset(info[0].As<Function>());

  return MaybeUnwrapOr(ref.New({}), Object());
}
}  // namespace

Object InitFunctionReference(Env env) {
  Object exports = Object::New(env);
  exports["CallWithRecvArgc"] = Function::New(env, CallWithRecvArgc);
  exports["CallWithRecvVector"] = Function::New(env, CallWithRecvVector);
  exports["CallWithRecvInitList"] = Function::New(env, CallWithRecvInitList);
  exports["CallWithInitList"] = Function::New(env, CallWithInitList);
  exports["CallWithVec"] = Function::New(env, CallWithVectorArgs);
  exports["ConstructWithMove"] =
      Function::New(env, ConstructRefFromExisitingRef);
  exports["AsyncCallWithInitList"] =
      Function::New(env, MakeAsyncCallbackWithInitList);
  exports["AsyncCallWithVector"] =
      Function::New(env, MakeAsyncCallbackWithVector);
  exports["AsyncCallWithArgv"] = Function::New(env, MakeAsyncCallbackWithArgv);
  exports["call"] = Function::New(env, Call);
  exports["construct"] = Function::New(env, Construct);

  return exports;
}
