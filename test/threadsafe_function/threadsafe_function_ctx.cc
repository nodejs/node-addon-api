#include "napi.h"

#if (NAPI_VERSION > 3)

namespace {

using namespace Napi;
using TSFNContext = Reference<Napi::Value>;

class TSFNWrap : public ObjectWrap<TSFNWrap> {
public:
  static Object Init(Napi::Env env, Object exports);
  TSFNWrap(const CallbackInfo &info);

  Napi::Value GetContext(const CallbackInfo & /*info*/) {
    Reference<Napi::Value> *ctx = _tsfn.GetContext();
    return ctx->Value();
  };

  Napi::Value GetContextByCall(const CallbackInfo &info) {
    Napi::Env env = info.Env();
    std::shared_ptr<Promise::Deferred> deferred =
        std::make_shared<Promise::Deferred>(env);
    _tsfn.BlockingCall([deferred](Napi::Env env, Function cb, void *ctx) {
      auto *ref = static_cast<Reference<Napi::Value> *>(ctx);
      deferred->Resolve(ref->Value());
    });
    return deferred->Promise();
  };

  Napi::Value Release(const CallbackInfo &info) {
    _tsfn.Release();
    return _deferred.Promise();
  };

private:
  ThreadSafeFunction _tsfn;
  Promise::Deferred _deferred;
};

Object TSFNWrap::Init(Napi::Env env, Object exports) {
  Function func = DefineClass(
      env, "TSFNWrap",
      {InstanceMethod("getContext", &TSFNWrap::GetContext),
       InstanceMethod("getContextByCall", &TSFNWrap::GetContextByCall),
       InstanceMethod("release", &TSFNWrap::Release)});

  exports.Set("TSFNWrap", func);
  return exports;
}

TSFNWrap::TSFNWrap(const CallbackInfo &info)
    : ObjectWrap<TSFNWrap>(info),
      _deferred(Promise::Deferred::New(info.Env())) {
  Napi::Env env = info.Env();

  TSFNContext *ctx = new TSFNContext;
  *ctx = Persistent(info[0]);

  _tsfn = ThreadSafeFunction::New(
      info.Env(), Function::New(env, [](const CallbackInfo & /*info*/) {}),
      Value(), "Test", 0, 1, ctx, [this](Napi::Env env, TSFNContext *ctx) {
        _deferred.Resolve(env.Undefined());
        ctx->Reset();
        delete ctx;
      });
}
} // namespace

Object InitThreadSafeFunctionCtx(Env env) {
  return TSFNWrap::Init(env, Object::New(env));
}

#endif
