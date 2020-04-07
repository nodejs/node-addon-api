#include "napi.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace {

// Context of our TSFN.
using TSFNContext = Reference<Napi::Value>;

// Data passed to ThreadSafeFunctionEx::[Non]BlockingCall
struct CallJsData {
  CallJsData(Napi::Env env) : deferred(Promise::Deferred::New(env)){};

  Promise::Deferred deferred;
};

// CallJs callback function
static void CallJs(Napi::Env /*env*/, Napi::Function /*jsCallback*/,
                   TSFNContext *context, CallJsData *data) {
  data->deferred.Resolve(context->Value());
}

// Full type of our ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<TSFNContext, CallJsData, CallJs>;

// A JS-accessible wrap that holds a TSFN.
class TSFNWrap : public ObjectWrap<TSFNWrap> {
public:
  static Object Init(Napi::Env env, Object exports);
  TSFNWrap(const CallbackInfo &info);

  Napi::Value GetContextByCall(const CallbackInfo &info) {
    Napi::Env env = info.Env();
    std::unique_ptr<CallJsData> callData = std::make_unique<CallJsData>(env);
    auto &deferred = callData->deferred;
    _tsfn.BlockingCall(callData.release());
    return deferred.Promise();
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

  TSFNContext *ctx = new Reference<Napi::Value>;
  *ctx = Persistent(info[0]);

  _tsfn = ThreadSafeFunctionEx<TSFNContext, CallJsData, CallJs>::New(
      info.Env(), // napi_env env,
      Function::New(
          env,
          [](const CallbackInfo & /*info*/) {}), // const Function& callback,
      Value(),                                   // const Object& resource,
      "Test",                                    // ResourceString resourceName,
      1,                                         // size_t maxQueueSize,
      1,                                         // size_t initialThreadCount,
      ctx,                                       // ContextType* context,

      [this](Napi::Env env, void *,
             TSFNContext *ctx) { // Finalizer finalizeCallback,
        _deferred.Resolve(env.Undefined());
        delete ctx;
      },
      static_cast<void *>(nullptr) // FinalizerDataType* data,
  );
}
} // namespace

Object InitThreadSafeFunctionEx(Env env) {
  return TSFNWrap::Init(env, Object::New(env));
}

#endif
