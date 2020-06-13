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

  auto env = info.Env();
#if NAPI_VERSION == 4
  // A threadsafe function on N-API 4 still requires a callback function.
  _tsfn =
      TSFN::New(env, // napi_env env,
                TSFN::DefaultFunctionFactory(
                    env), // N-API 5+: nullptr; else: const Function& callback,
                "Test",   // ResourceString resourceName,
                1,        // size_t maxQueueSize,
                1         // size_t initialThreadCount
      );
#else
  _tsfn = TSFN::New(env,    // napi_env env,
                    "Test", // ResourceString resourceName,
                    1,      // size_t maxQueueSize,
                    1       // size_t initialThreadCount
  );
#endif
}
} // namespace

Object InitThreadSafeFunctionExSimple(Env env) {
  return TSFNWrap::Init(env, Object::New(env));
}

#endif
