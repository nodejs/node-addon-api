#include "napi.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace {

class TSFNWrap : public ObjectWrap<TSFNWrap> {
public:
  static Object Init(Napi::Env env, Object exports);
  TSFNWrap(const CallbackInfo &info);

  Napi::Value GetContext(const CallbackInfo & /*info*/) {
    Reference<Napi::Value> *ctx = _tsfn.GetContext();
    return ctx->Value();
  };

  Napi::Value Release(const CallbackInfo &info) {
    Napi::Env env = info.Env();
    _deferred = std::unique_ptr<Promise::Deferred>(new Promise::Deferred(env));
    _tsfn.Release();
    return _deferred->Promise();
  };

private:
  ThreadSafeFunction _tsfn;
  std::unique_ptr<Promise::Deferred> _deferred;
};

Object TSFNWrap::Init(Napi::Env env, Object exports) {
  Function func =
      DefineClass(env, "TSFNWrap",
                  {InstanceMethod("getContext", &TSFNWrap::GetContext),
                   InstanceMethod("release", &TSFNWrap::Release)});

  exports.Set("TSFNWrap", func);
  return exports;
}

TSFNWrap::TSFNWrap(const CallbackInfo &info) : ObjectWrap<TSFNWrap>(info) {
  Napi::Env env = info.Env();

  Reference<Napi::Value> *_ctx = new Reference<Napi::Value>;
  *_ctx = Persistent(info[0]);

  _tsfn = ThreadSafeFunction::New(
      info.Env(), Function::New(env, [](const CallbackInfo & /*info*/) {}),
      Object::New(env), "Test", 1, 1, _ctx,
      [this](Napi::Env env, Reference<Napi::Value> *ctx) {
        _deferred->Resolve(env.Undefined());
        ctx->Reset();
        delete ctx;
      });
}

} // namespace

Object InitThreadSafeFunctionCtx(Env env) {
  return TSFNWrap::Init(env, Object::New(env));
}

#endif
