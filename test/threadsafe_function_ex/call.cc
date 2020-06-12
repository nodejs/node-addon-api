#include "napi.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace {

// Context of our TSFN.
using TSFNContext = void;

// Data passed (as pointer) to ThreadSafeFunctionEx::[Non]BlockingCall
struct TSFNData {
  Reference<Napi::Value> data;
  Promise::Deferred deferred;
};

// CallJs callback function
static void CallJs(Napi::Env env, Napi::Function jsCallback,
                   TSFNContext * /*context*/, TSFNData *data) {
  if (!(env == nullptr || jsCallback == nullptr)) {
    if (data != nullptr) {
      jsCallback.Call(env.Undefined(), {data->data.Value()});
      data->deferred.Resolve(data->data.Value());
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

  Napi::Value DoCall(const CallbackInfo &info) {
    Napi::Env env = info.Env();
    TSFNData *data =
        new TSFNData{Napi::Reference<Napi::Value>(Persistent(info[0])),
                     Promise::Deferred::New(env)};
    _tsfn.NonBlockingCall(data);
    return data->deferred.Promise();
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
  Function func = DefineClass(env, "TSFNWrap",
                              {InstanceMethod("doCall", &TSFNWrap::DoCall),
                               InstanceMethod("release", &TSFNWrap::Release)});

  exports.Set("TSFNWrap", func);
  return exports;
}

TSFNWrap::TSFNWrap(const CallbackInfo &info)
    : ObjectWrap<TSFNWrap>(info),
      _deferred(Promise::Deferred::New(info.Env())) {
  Napi::Env env = info.Env();
  Function callback = info[0].As<Function>();

  _tsfn = TSFN::New(env,      // napi_env env,
                    callback, // const Function& callback,
                    Value(),  // const Object& resource,
                    "Test",   // ResourceString resourceName,
                    0,        // size_t maxQueueSize,
                    1         // size_t initialThreadCount,
  );
}
} // namespace

Object InitThreadSafeFunctionExCall(Env env) {
  return TSFNWrap::Init(env, Object::New(env));
}

#endif
