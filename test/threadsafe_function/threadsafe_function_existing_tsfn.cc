#include "napi.h"
#include <cstdlib>

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace {

struct TestContext {
  TestContext(Promise::Deferred &&deferred) : deferred(std::move(deferred)){};
  napi_threadsafe_function tsfn;
  Promise::Deferred deferred;
};

void FinalizeCB(napi_env /*env*/, void *context, void * /*finalizedata*/) {
  TestContext *testContext = static_cast<TestContext *>(context);
  delete testContext;
}

void CallJSWithData(napi_env env, napi_value value, void *context, void *data) {
  TestContext *testContext = static_cast<TestContext *>(context);
  double *dblVal = static_cast<double *>(data);
  Function(env, value).Call({Number::New(env, *dblVal)});
  delete dblVal;

  napi_status status =
      napi_release_threadsafe_function(testContext->tsfn, napi_tsfn_release);

  NAPI_THROW_IF_FAILED_VOID(env, status);

  testContext->deferred.Resolve(Env(env).Undefined());
}

void CallJSNoData(napi_env env, napi_value value, void *context,
                  void * /*data*/) {
  TestContext *testContext = static_cast<TestContext *>(context);
  Value undef = Env(env).Undefined();
  Function(env, value).Call({undef});

  napi_status status =
      napi_release_threadsafe_function(testContext->tsfn, napi_tsfn_release);

  NAPI_THROW_IF_FAILED_VOID(env, status);

  testContext->deferred.Resolve(undef);
}

static Value TestCall(const CallbackInfo &info) {
  Napi::Env env = info.Env();
  bool isBlocking = false;
  bool hasData = false;
  if (info.Length() > 0) {
    Object opts = info[0].As<Object>();
    if (opts.Has("blocking")) {
      isBlocking = opts.Get("blocking").ToBoolean();
    }
    if (opts.Has("data")) {
      hasData = opts.Get("data").ToBoolean();
    }
  }

  // Allow optional callback passed from JS. Useful for testing.
  Function cb = info.Length() > 1
                    ? info[1].As<Function>()
                    : Function::New(env, [](const CallbackInfo &/*info*/) {});

  TestContext *testContext = new TestContext(Napi::Promise::Deferred(env));

  napi_status status = napi_create_threadsafe_function(
      env, cb, Object::New(env), String::New(env, "Test"), 0, 1,
      nullptr, /*finalize data*/
      FinalizeCB, testContext, hasData ? CallJSWithData : CallJSNoData,
      &testContext->tsfn);

  NAPI_THROW_IF_FAILED(env, status, Value());

  ThreadSafeFunction wrapped = ThreadSafeFunction(testContext->tsfn);

  // Test the four napi_threadsafe_function direct-accessing calls
  if (isBlocking) {
    if (hasData) {
      wrapped.BlockingCall(static_cast<void *>(new double(std::rand())));
    } else {
      wrapped.BlockingCall(nullptr);
    }
  } else {
    if (hasData) {
      wrapped.NonBlockingCall(static_cast<void *>(new double(std::rand())));
    } else {
      wrapped.NonBlockingCall(nullptr);
    }
  }

  return testContext->deferred.Promise();
}

} // namespace

Object InitThreadSafeFunctionExistingTsfn(Env env) {
  Object exports = Object::New(env);
  exports["testCall"] = Function::New(env, TestCall);

  return exports;
}

#endif
