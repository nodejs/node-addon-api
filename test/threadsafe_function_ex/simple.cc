#include "napi.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace {

// Full type of our ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<>;

// A JS-accessible wrap that holds a TSFN.
class TSFNWrap : public ObjectWrap<TSFNWrap> {
public:
  static Object Init(Napi::Env env, Object exports);
  TSFNWrap(const CallbackInfo &info);

  Napi::Value Release(const CallbackInfo &) {
    _tsfn.Release();
    return _deferred.Promise();
  };

  Napi::Value Call(const CallbackInfo &info) {
    _tsfn.NonBlockingCall();
    return info.Env().Undefined();
  };

private:
  TSFN _tsfn;
  Promise::Deferred _deferred;
};

Object TSFNWrap::Init(Napi::Env env, Object exports) {
  Function func = DefineClass(env, "TSFNWrap",
                              {InstanceMethod("call", &TSFNWrap::Call),
                               InstanceMethod("release", &TSFNWrap::Release)});

  exports.Set("TSFNWrap", func);
  return exports;
}

TSFNWrap::TSFNWrap(const CallbackInfo &info)
    : ObjectWrap<TSFNWrap>(info),
      _deferred(Promise::Deferred::New(info.Env())) {

  _tsfn = TSFN::New(
      info.Env(),                   // napi_env env,
      Function(),                   // const Function& callback,
      Value(),                      // const Object& resource,
      "Test",                       // ResourceString resourceName,
      1,                            // size_t maxQueueSize,
      1,                            // size_t initialThreadCount,
      static_cast<void *>(nullptr), // ContextType* context,
      [this](Napi::Env env,         // Finalizer finalizeCallback,
             void * /*data*/,
             void * /*ctx*/) { _deferred.Resolve(env.Undefined()); },
      static_cast<void *>(nullptr) // FinalizerDataType* data,
  );
}
} // namespace

Object InitThreadSafeFunctionExSimple(Env env) {
  return TSFNWrap::Init(env, Object::New(env));
}

#endif
