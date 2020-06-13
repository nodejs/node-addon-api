#include "napi.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace simple {

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
} // namespace simple

namespace existing {

struct DataType {
  Promise::Deferred deferred;
  bool reject;
};

// CallJs callback function provided to `napi_create_threadsafe_function`. It is
// _NOT_ used by `Napi::ThreadSafeFunctionEx<>`.
static void CallJs(napi_env env, napi_value jsCallback, void * /*context*/,
                   void *data) {
  DataType *casted = static_cast<DataType *>(data);
  if (env != nullptr) {
    if (data != nullptr) {
      napi_value undefined;
      napi_status status = napi_get_undefined(env, &undefined);
      NAPI_THROW_IF_FAILED(env, status);
      if (casted->reject) {
        casted->deferred.Reject(undefined);
      } else {
        casted->deferred.Resolve(undefined);
      }
    }
  }
  if (casted != nullptr) {
    delete casted;
  }
}

// Full type of our ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<void, DataType>;

// A JS-accessible wrap that holds a TSFN.
class ExistingTSFNWrap : public ObjectWrap<ExistingTSFNWrap> {
public:
  static Object Init(Napi::Env env, Object exports);
  ExistingTSFNWrap(const CallbackInfo &info);

  Napi::Value Release(const CallbackInfo &) {
    _tsfn.Release();
    return _deferred.Promise();
  };

  Napi::Value Call(const CallbackInfo &info) {
    auto *data =
        new DataType{Promise::Deferred::New(info.Env()), info[0].ToBoolean()};
    _tsfn.NonBlockingCall(data);
    return data->deferred.Promise();
  };

private:
  TSFN _tsfn;
  Promise::Deferred _deferred;
};

Object ExistingTSFNWrap::Init(Napi::Env env, Object exports) {
  Function func =
      DefineClass(env, "ExistingTSFNWrap",
                  {InstanceMethod("call", &ExistingTSFNWrap::Call),
                   InstanceMethod("release", &ExistingTSFNWrap::Release)});

  exports.Set("ExistingTSFNWrap", func);
  return exports;
}

ExistingTSFNWrap::ExistingTSFNWrap(const CallbackInfo &info)
    : ObjectWrap<ExistingTSFNWrap>(info),
      _deferred(Promise::Deferred::New(info.Env())) {

  auto env = info.Env();
#if NAPI_VERSION == 4
  napi_threadsafe_function napi_tsfn;
  auto status = napi_create_threadsafe_function(
      info.Env(), TSFN::DefaultFunctionFactory(env), nullptr,
      String::From(info.Env(), "Test"), 0, 1, nullptr, nullptr, nullptr, CallJs,
      &napi_tsfn);
  if (status != napi_ok) {
    NAPI_THROW_IF_FAILED(env, status);
  }
  // A threadsafe function on N-API 4 still requires a callback function.
  _tsfn = TSFN(napi_tsfn);
#else
  napi_threadsafe_function napi_tsfn;
  auto status = napi_create_threadsafe_function(
      info.Env(), nullptr, nullptr, String::From(info.Env(), "Test"), 0, 1,
      nullptr, nullptr, nullptr, CallJs, &napi_tsfn);
  if (status != napi_ok) {
    NAPI_THROW_IF_FAILED(env, status);
  }
  _tsfn = TSFN(napi_tsfn);
#endif
}
} // namespace existing

namespace empty {
#if NAPI_VERSION > 4

using Context = void;

struct DataType {
  Promise::Deferred deferred;
  bool reject;
};

// CallJs callback function
static void CallJs(Napi::Env env, Function jsCallback, Context * /*context*/,
                   DataType *data) {
  if (env != nullptr) {
    if (data != nullptr) {
      if (data->reject) {
        data->deferred.Reject(env.Undefined());
      } else {
        data->deferred.Resolve(env.Undefined());
      }
    }
  }
  if (data != nullptr) {
    delete data;
  }
}

// Full type of our ThreadSafeFunctionEx
using EmptyTSFN = ThreadSafeFunctionEx<void, DataType, CallJs>;

// A JS-accessible wrap that holds a TSFN.
class EmptyTSFNWrap : public ObjectWrap<EmptyTSFNWrap> {
public:
  static Object Init(Napi::Env env, Object exports);
  EmptyTSFNWrap(const CallbackInfo &info);

  Napi::Value Release(const CallbackInfo &) {
    _tsfn.Release();
    return _deferred.Promise();
  };

  Napi::Value Call(const CallbackInfo &info) {
    if (info.Length() == 0 || !info[0].IsBoolean()) {
      NAPI_THROW(
          Napi::TypeError::New(info.Env(), "Expected argument 0 to be boolean"),
          Value());
    }

    auto *data =
        new DataType{Promise::Deferred::New(info.Env()), info[0].ToBoolean()};
    _tsfn.NonBlockingCall(data);
    return data->deferred.Promise();
  };

private:
  EmptyTSFN _tsfn;
  Promise::Deferred _deferred;
};

Object EmptyTSFNWrap::Init(Napi::Env env, Object exports) {
  Function func =
      DefineClass(env, "EmptyTSFNWrap",
                  {InstanceMethod("call", &EmptyTSFNWrap::Call),
                   InstanceMethod("release", &EmptyTSFNWrap::Release)});

  exports.Set("EmptyTSFNWrap", func);
  return exports;
}

EmptyTSFNWrap::EmptyTSFNWrap(const CallbackInfo &info)
    : ObjectWrap<EmptyTSFNWrap>(info),
      _deferred(Promise::Deferred::New(info.Env())) {

  auto env = info.Env();
  _tsfn = EmptyTSFN::New(env,    // napi_env env,
                         "Test", // ResourceString resourceName,
                         0,      // size_t maxQueueSize,
                         1       // size_t initialThreadCount
  );
}
#endif
} // namespace empty
Object InitThreadSafeFunctionExSimple(Env env) {

#if NAPI_VERSION > 4
  return empty::EmptyTSFNWrap::Init(
      env, existing::ExistingTSFNWrap::Init(
               env, simple::TSFNWrap::Init(env, Object::New(env))));
#else
  return existing::ExistingTSFNWrap::Init(
      env, simple::TSFNWrap::Init(env, Object::New(env)));
#endif
}

#endif
