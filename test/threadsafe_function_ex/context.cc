#include "napi.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace {

// Context of our TSFN.
using TSFNContext = Reference<Napi::Value>;

// Data passed (as pointer) to ThreadSafeFunctionEx::[Non]BlockingCall
using TSFNData = Promise::Deferred;

// CallJs callback function
static void CallJs(Napi::Env env, Napi::Function /*jsCallback*/,
                   TSFNContext *context, TSFNData *data) {
  if (env != nullptr) {
    if (data != nullptr) {
      data->Resolve(context->Value());
    }
  }
  if (data != nullptr) {
    delete data;
  }
}

// Full type of our ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<TSFNContext, TSFNData, CallJs>;

// A JS-accessible wrap that holds a TSFN.
class TSFNWrap : public ObjectWrap<TSFNWrap> {
public:
  static Object Init(Napi::Env env, Object exports);
  TSFNWrap(const CallbackInfo &info);

  Napi::Value GetContextByCall(const CallbackInfo &info) {
    Napi::Env env = info.Env();
    auto *callData = new TSFNData(env);
    _tsfn.NonBlockingCall(callData);
    return callData->Promise();
  };

  Napi::Value GetContextFromTsfn(const CallbackInfo &) {
    return _tsfn.GetContext()->Value();
  };

  Napi::Value Release(const CallbackInfo &) {
    _tsfn.Release();
    return _deferred.Promise();
  };

private:
  TSFN _tsfn;
  Promise::Deferred _deferred;
};

Object TSFNWrap::Init(Napi::Env env, Object exports) {
  Function func = DefineClass(
      env, "TSFNWrap",
      {InstanceMethod("getContextByCall", &TSFNWrap::GetContextByCall),
       InstanceMethod("getContextFromTsfn", &TSFNWrap::GetContextFromTsfn),
       InstanceMethod("release", &TSFNWrap::Release)});

  exports.Set("TSFNWrap", func);
  return exports;
}

TSFNWrap::TSFNWrap(const CallbackInfo &info)
    : ObjectWrap<TSFNWrap>(info),
      _deferred(Promise::Deferred::New(info.Env())) {
  Napi::Env env = info.Env();

  TSFNContext *context = new TSFNContext(Persistent(info[0]));

  _tsfn = TSFN::New(
      info.Env(), // napi_env env,
      Function::New(
          env,
          [](const CallbackInfo & /*info*/) {}), // const Function& callback,
      Value(),                                   // const Object& resource,
      "Test",                                    // ResourceString resourceName,
      1,                                         // size_t maxQueueSize,
      1,                                         // size_t initialThreadCount,
      context,                                   // ContextType* context,

      [this](Napi::Env env, void *,
             TSFNContext *ctx) { // Finalizer finalizeCallback,
        _deferred.Resolve(env.Undefined());
        delete ctx;
      },
      static_cast<void *>(nullptr) // FinalizerDataType* data,
  );
}
} // namespace

Object InitThreadSafeFunctionExContext(Env env) {
  return TSFNWrap::Init(env, Object::New(env));
}

#endif
