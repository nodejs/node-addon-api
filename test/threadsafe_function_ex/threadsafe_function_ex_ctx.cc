#include "napi.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

using ContextType = Reference<Napi::Value>;
using TSFN = ThreadSafeFunctionEx<ContextType>;

namespace {

class TSFNWrap : public ObjectWrap<TSFNWrap> {
public:
  static Object Init(Napi::Env env, Object exports);
  TSFNWrap(const CallbackInfo &info);

  Napi::Value GetContext(const CallbackInfo & /*info*/) {
    ContextType *ctx = _tsfn.GetContext();
    return ctx->Value();
  };

  Napi::Value Release(const CallbackInfo &info) {
    Napi::Env env = info.Env();
    _deferred = std::unique_ptr<Promise::Deferred>(new Promise::Deferred(env));
    _tsfn.Release();
    return _deferred->Promise();
  };

private:
  TSFN _tsfn;
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
  ContextType *_ctx = new ContextType;
  *_ctx = Persistent(info[0]);

  _tsfn = TSFN::New(info.Env(), this->Value(), "Test", 1, 1, _ctx,
                    [this](Napi::Env env, void *, ContextType *ctx) {
                      _deferred->Resolve(env.Undefined());
                      ctx->Reset();
                      delete ctx;
                    });
}

} // namespace

Object InitThreadSafeFunctionExCtx(Env env) {
  return TSFNWrap::Init(env, Object::New(env));
}

#endif
