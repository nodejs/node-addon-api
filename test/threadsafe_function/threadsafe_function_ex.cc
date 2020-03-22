#include "napi.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

using TSFNContext = Reference<Napi::Value>;

namespace {

struct CallJsData {
  CallJsData(Napi::Env env) : deferred(Promise::Deferred::New(env)) { };

  void resolve(TSFNContext* context) {
    deferred.Resolve(context->Value());
  };
  Promise::Deferred deferred;
};

class TSFNWrap : public ObjectWrap<TSFNWrap> {
public:
  static Object Init(Napi::Env env, Object exports);
  TSFNWrap(const CallbackInfo &info);

  Napi::Value GetContextByCall(const CallbackInfo &info) {
    Napi::Env env = info.Env();
    std::unique_ptr<CallJsData> callData = std::make_unique<CallJsData>(env);
    auto& deferred = callData->deferred;
    _tsfn.BlockingCall(callData.release());
    return deferred.Promise();
  };

  Napi::Value GetContextFromTsfn(const CallbackInfo &info) {
    return _tsfn.GetContext()->Value();
  };

  Napi::Value Release(const CallbackInfo &info) {
    _tsfn.Release();
    return _deferred.Promise();
  };

private:
  ThreadSafeFunctionEx<TSFNContext> _tsfn;
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

  TSFNContext *ctx = new Reference<Napi::Value>;
  *ctx = Persistent(info[0]);

  _tsfn = ThreadSafeFunctionEx<TSFNContext>::New(
      info.Env(), // napi_env env,
      Function::New(
          env,
          [](const CallbackInfo & /*info*/) {}), // const Function& callback,
      Value(),                                   // const Object& resource,
      "Test",                                    // ResourceString resourceName,
      1,                                         // size_t maxQueueSize,
      1,                                         // size_t initialThreadCount,
      ctx,                                       // ContextType* context,

      [this](Napi::Env env, void*, TSFNContext *ctx) {  // Finalizer finalizeCallback,
        _deferred.Resolve(env.Undefined());
        delete ctx;
      },
      static_cast<void*>(nullptr),                                 // FinalizerDataType* data,
      [](napi_env env, napi_value js_callback, void *context, void *data) { // call_js_cb
        std::unique_ptr<CallJsData> callData(static_cast<CallJsData*>(data));
        callData->resolve(static_cast<TSFNContext*>(context));
      });
}
} // namespace

Object InitThreadSafeFunctionEx(Env env) {
  return TSFNWrap::Init(env, Object::New(env));
}

#endif
