#include "napi.h"
#include "test_helper.h"

using namespace Napi;

Value IsPromise(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsPromise());
}

Value ResolvePromise(const CallbackInfo& info) {
  auto deferred = Promise::Deferred::New(info.Env());
  deferred.Resolve(info[0]);
  return deferred.Promise();
}

Value RejectPromise(const CallbackInfo& info) {
  auto deferred = Promise::Deferred::New(info.Env());
  deferred.Reject(info[0]);
  return deferred.Promise();
}

Value PromiseReturnsCorrectEnv(const CallbackInfo& info) {
  auto deferred = Promise::Deferred::New(info.Env());
  return Boolean::New(info.Env(), deferred.Env() == info.Env());
}

Value ThenMethodOnFulfilled(const CallbackInfo& info) {
  auto deferred = Promise::Deferred::New(info.Env());
  Function onFulfilled = info[0].As<Function>();

  Promise resultPromise = MaybeUnwrap(deferred.Promise().Then(onFulfilled));

  bool isPromise = resultPromise.IsPromise();
  deferred.Resolve(Number::New(info.Env(), 42));

  Object result = Object::New(info.Env());
  result["isPromise"] = Boolean::New(info.Env(), isPromise);
  result["promise"] = resultPromise;

  return result;
}

Value ThenMethodOnFulfilledOnRejectedResolve(const CallbackInfo& info) {
  auto deferred = Promise::Deferred::New(info.Env());
  Function onFulfilled = info[0].As<Function>();
  Function onRejected = info[1].As<Function>();

  Promise resultPromise =
      MaybeUnwrap(deferred.Promise().Then(onFulfilled, onRejected));

  bool isPromise = resultPromise.IsPromise();
  deferred.Resolve(Number::New(info.Env(), 42));

  Object result = Object::New(info.Env());
  result["isPromise"] = Boolean::New(info.Env(), isPromise);
  result["promise"] = resultPromise;

  return result;
}

Value ThenMethodOnFulfilledOnRejectedReject(const CallbackInfo& info) {
  auto deferred = Promise::Deferred::New(info.Env());
  Function onFulfilled = info[0].As<Function>();
  Function onRejected = info[1].As<Function>();

  Promise resultPromise =
      MaybeUnwrap(deferred.Promise().Then(onFulfilled, onRejected));

  bool isPromise = resultPromise.IsPromise();
  deferred.Reject(String::New(info.Env(), "Rejected"));

  Object result = Object::New(info.Env());
  result["isPromise"] = Boolean::New(info.Env(), isPromise);
  result["promise"] = resultPromise;

  return result;
}

Value CatchMethod(const CallbackInfo& info) {
  auto deferred = Promise::Deferred::New(info.Env());
  Function onRejected = info[0].As<Function>();

  Promise resultPromise = MaybeUnwrap(deferred.Promise().Catch(onRejected));

  bool isPromise = resultPromise.IsPromise();
  deferred.Reject(String::New(info.Env(), "Rejected"));

  Object result = Object::New(info.Env());
  result["isPromise"] = Boolean::New(info.Env(), isPromise);
  result["promise"] = resultPromise;

  return result;
}

Object InitPromise(Env env) {
  Object exports = Object::New(env);

  exports["isPromise"] = Function::New(env, IsPromise);
  exports["resolvePromise"] = Function::New(env, ResolvePromise);
  exports["rejectPromise"] = Function::New(env, RejectPromise);
  exports["promiseReturnsCorrectEnv"] =
      Function::New(env, PromiseReturnsCorrectEnv);
  exports["thenMethodOnFulfilled"] = Function::New(env, ThenMethodOnFulfilled);
  exports["thenMethodOnFulfilledOnRejectedResolve"] =
      Function::New(env, ThenMethodOnFulfilledOnRejectedResolve);
  exports["thenMethodOnFulfilledOnRejectedReject"] =
      Function::New(env, ThenMethodOnFulfilledOnRejectedReject);
  exports["catchMethod"] = Function::New(env, CatchMethod);

  return exports;
}
