#include "../util/util.h"
#include "napi.h"
#include <array>

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace call {

// Context of the TSFN.
using ContextType = std::nullptr_t;

// Data passed (as pointer) to [Non]BlockingCall
struct DataType {
  Reference<Napi::Value> data;
  Promise::Deferred deferred;
};

// CallJs callback function
static void CallJs(Napi::Env env, Napi::Function jsCallback,
                   ContextType * /*context*/, DataType *data) {
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

// Full type of the ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<ContextType, DataType, CallJs>;

class TSFNWrap;
using base = tsfnutil::TSFNWrapBase<TSFNWrap, ContextType, TSFN>;

// A JS-accessible wrap that holds the TSFN.
class TSFNWrap : public base {
public:
  TSFNWrap(const CallbackInfo &info) : base(info) {
    Napi::Env env = info.Env();
    _tsfn = TSFN::New(env,                    // napi_env env,
                      info[0].As<Function>(), // const Function& callback,
                      "Test",                 // ResourceString resourceName,
                      0,                      // size_t maxQueueSize,
                      1,                      // size_t initialThreadCount,
                      nullptr,                // ContextType* context
                      base::Finalizer,        // Finalizer finalizer
                      &_deferred              // FinalizerDataType* data
    );
  }

  static std::array<ClassPropertyDescriptor<TSFNWrap>, 2> InstanceMethods() {
    return {{InstanceMethod("release", &TSFNWrap::Release),
             InstanceMethod("call", &TSFNWrap::Call)}};
  }

  Napi::Value Call(const CallbackInfo &info) {
    Napi::Env env = info.Env();
    DataType *data =
        new DataType{Napi::Reference<Napi::Value>(Persistent(info[0])),
                     Promise::Deferred::New(env)};
    _tsfn.NonBlockingCall(data);
    return data->deferred.Promise();
  };
};

} // namespace call

namespace context {

// Context of the TSFN.
using ContextType = Reference<Napi::Value>;

// Data passed (as pointer) to [Non]BlockingCall
using DataType = Promise::Deferred;

// CallJs callback function
static void CallJs(Napi::Env env, Napi::Function /*jsCallback*/,
                   ContextType *context, DataType *data) {
  if (env != nullptr) {
    if (data != nullptr) {
      data->Resolve(context->Value());
    }
  }
  if (data != nullptr) {
    delete data;
  }
}

// Full type of the ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<ContextType, DataType, CallJs>;

class TSFNWrap;
using base = tsfnutil::TSFNWrapBase<TSFNWrap, ContextType, TSFN>;

// A JS-accessible wrap that holds the TSFN.
class TSFNWrap : public base {
public:
  TSFNWrap(const CallbackInfo &info) : base(info) {
    Napi::Env env = info.Env();

    ContextType *context = new ContextType(Persistent(info[0]));

    _tsfn =
        TSFN::New(env,                             // napi_env env,
                  TSFN::EmptyFunctionFactory(env), // const Function& callback,
                  Value(),                         // const Object& resource,
                  "Test",          // ResourceString resourceName,
                  0,               // size_t maxQueueSize,
                  1,               // size_t initialThreadCount,
                  context,         // ContextType* context,
                  base::Finalizer, // Finalizer finalizer
                  &_deferred       // FinalizerDataType* data
        );
  }

  static std::array<ClassPropertyDescriptor<TSFNWrap>, 3> InstanceMethods() {
    return {{InstanceMethod("call", &TSFNWrap::Call),
             InstanceMethod("getContext", &TSFNWrap::GetContext),
             InstanceMethod("release", &TSFNWrap::Release)}};
  }

  Napi::Value Call(const CallbackInfo &info) {
    auto *callData = new DataType(info.Env());
    _tsfn.NonBlockingCall(callData);
    return callData->Promise();
  };

  Napi::Value GetContext(const CallbackInfo &) {
    return _tsfn.GetContext()->Value();
  };
};
} // namespace context

namespace empty {
#if NAPI_VERSION > 4

// Context of the TSFN.
using ContextType = std::nullptr_t;

// Data passed (as pointer) to [Non]BlockingCall
struct DataType {
  Promise::Deferred deferred;
};

// CallJs callback function
static void CallJs(Napi::Env env, Function jsCallback,
                   ContextType * /*context*/, DataType *data) {
  if (env != nullptr) {
    if (data != nullptr) {
      if (jsCallback.IsEmpty()) {
        data->deferred.Resolve(Boolean::New(env, true));
      } else {
        data->deferred.Reject(String::New(env, "jsCallback is not empty"));
      }
    }
  }
  if (data != nullptr) {
    delete data;
  }
}

// Full type of the ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<ContextType, DataType, CallJs>;

class TSFNWrap;
using base = tsfnutil::TSFNWrapBase<TSFNWrap, ContextType, TSFN>;

// A JS-accessible wrap that holds the TSFN.
class TSFNWrap : public base {
public:
  TSFNWrap(const CallbackInfo &info) : base(info) {

    auto env = info.Env();
    _tsfn = TSFN::New(env,             // napi_env env,
                      "Test",          // ResourceString resourceName,
                      0,               // size_t maxQueueSize,
                      1,               // size_t initialThreadCount,
                      nullptr,         // ContextType* context
                      base::Finalizer, // Finalizer finalizer
                      &_deferred       // FinalizerDataType* data
    );
  }

  static std::array<ClassPropertyDescriptor<TSFNWrap>, 2> InstanceMethods() {
    return {{InstanceMethod("release", &TSFNWrap::Release),
             InstanceMethod("call", &TSFNWrap::Call)}};
  }

  Napi::Value Call(const CallbackInfo &info) {
    auto data = new DataType{Promise::Deferred::New(info.Env())};
    _tsfn.NonBlockingCall(data);
    return data->deferred.Promise();
  };
};

#endif
} // namespace empty

namespace existing {

// Data passed (as pointer) to [Non]BlockingCall
struct DataType {
  Promise::Deferred deferred;
  bool reject;
};

// CallJs callback function provided to `napi_create_threadsafe_function`. It is
// _NOT_ used by `Napi::ThreadSafeFunctionEx<>`, which is why these arguments
// are napi_*.
static void CallJs(napi_env env, napi_value jsCallback, void * /*context*/,
                   void *data) {
  DataType *casted = static_cast<DataType *>(data);
  if (env != nullptr) {
    if (jsCallback != nullptr) {
      Function(env, jsCallback).Call(0, nullptr);
    }
    if (data != nullptr) {
      if (casted->reject) {
        casted->deferred.Reject(
            String::New(env, "The CallJs has rejected the promise"));
      } else {
        casted->deferred.Resolve(
            String::New(env, "The CallJs has resolved the promise"));
      }
    }
  }
  if (casted != nullptr) {
    delete casted;
  }
}

// This test creates a native napi_threadsafe_function itself, whose `context`
// parameter is the `TSFNWrap` object. We forward-declare, so we can use
// it as an argument inside `ThreadSafeFunctionEx<>`. This also allows us to
// statically get the correct type when using `tsfn.GetContext()`. The converse
// is true: if the ContextType does _not_ match that provided to the underlying
// napi_create_threadsafe_function, then the static type will be incorrect.
class TSFNWrap;

// Context of the TSFN.
using ContextType = TSFNWrap;

// Full type of our ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<ContextType, DataType>;
using base = tsfnutil::TSFNWrapBase<TSFNWrap, ContextType, TSFN>;

// A JS-accessible wrap that holds a TSFN.
class TSFNWrap : public base {
public:
  TSFNWrap(const CallbackInfo &info) : base(info) {

    auto env = info.Env();

    if (info.Length() < 1 || !info[0].IsFunction()) {
      NAPI_THROW_VOID(Napi::TypeError::New(
          env, "Invalid arguments: Expected arg0 = function"));
    }

    napi_threadsafe_function napi_tsfn;

    // A threadsafe function on N-API 4 still requires a callback function, so
    // this uses the `EmptyFunctionFactory` helper method to return a no-op
    // Function on N-API 5+.
    auto status = napi_create_threadsafe_function(
        info.Env(), info[0], nullptr, String::From(info.Env(), "Test"), 0, 1,
        nullptr, Finalizer, this, CallJs, &napi_tsfn);
    if (status != napi_ok) {
      NAPI_THROW_VOID(Error::New(env, "Could not create TSFN."));
    }
    _tsfn = TSFN(napi_tsfn);
  }

  static std::array<ClassPropertyDescriptor<TSFNWrap>, 2> InstanceMethods() {
    return {{InstanceMethod("release", &TSFNWrap::Release),
             InstanceMethod("call", &TSFNWrap::Call)}};
  }

  Napi::Value Call(const CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
      NAPI_THROW(Napi::TypeError::New(
                     env, "Invalid arguments: Expected arg0 = number [0,5]"),
                 Value());
    }
    auto arg0 = info[0];
    if (!arg0.IsNumber()) {
      NAPI_THROW(Napi::TypeError::New(
                     env, "Invalid arguments: Expected arg0 = number [0,5]"),
                 Value());
    }
    auto mode = info[0].ToNumber().Int32Value();
    switch (mode) {
    // Use node-addon-api to send a call that either resolves or rejects the
    // promise in the data.
    case 0:
    case 1: {
      auto *data = new DataType{Promise::Deferred::New(env), mode == 1};
      _tsfn.NonBlockingCall(data);
      return data->deferred.Promise();
    }
    // Use node-addon-api to send a call with no data
    case 2: {
      _tsfn.NonBlockingCall();
      return Boolean::New(env, true);
    }
    // Use napi to send a call that either resolves or rejects the promise in
    // the data.
    case 3:
    case 4: {
      auto *data = new DataType{Promise::Deferred::New(env), mode == 4};
      napi_call_threadsafe_function(_tsfn, data, napi_tsfn_nonblocking);
      return data->deferred.Promise();
    }
    // Use napi to send a call with no data
    case 5: {
      napi_call_threadsafe_function(_tsfn, nullptr, napi_tsfn_nonblocking);
      return Boolean::New(env, true);
    }
    }
    NAPI_THROW(Napi::TypeError::New(
                   env, "Invalid arguments: Expected arg0 = number [0,5]"),
               Value());
  };

private:
  // This test uses a custom napi (NOT node-addon-api) TSFN finalizer.
  static void Finalizer(napi_env env, void * /*data*/, void *ctx) {
    TSFNWrap *tsfn = static_cast<TSFNWrap *>(ctx);
    tsfn->Finalizer(env);
  }

  // Clean up the TSFNWrap by resolving the promise.
  void Finalizer(napi_env e) {
    if (_deferred) {
      _deferred->Resolve(Boolean::New(e, true));
      _deferred.release();
    }
  }
};

} // namespace existing
namespace simple {

using ContextType = std::nullptr_t;

// Full type of our ThreadSafeFunctionEx. We don't specify the `ContextType`
// here (even though the _default_ for the type argument is `std::nullptr_t`) to
// demonstrate construction with no type arguments.
using TSFN = ThreadSafeFunctionEx<>;

class TSFNWrap;
using base = tsfnutil::TSFNWrapBase<TSFNWrap, ContextType, TSFN>;

// A JS-accessible wrap that holds a TSFN.
class TSFNWrap : public base {
public:
  TSFNWrap(const CallbackInfo &info) : base(info) {

    auto env = info.Env();
#if NAPI_VERSION == 4
    // A threadsafe function on N-API 4 still requires a callback function.
    _tsfn = TSFN::New(
        env, // napi_env env,
        TSFN::EmptyFunctionFactory(
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

  static std::array<ClassPropertyDescriptor<TSFNWrap>, 2> InstanceMethods() {
    return {{InstanceMethod("release", &TSFNWrap::Release),
             InstanceMethod("call", &TSFNWrap::Call)}};
  }

  // Since this test spec has no CALLBACK, CONTEXT, or FINALIZER. We have no way
  // to know when the underlying ThreadSafeFunction has been finalized.
  Napi::Value Release(const CallbackInfo &info) {
    _tsfn.Release();
    return String::New(info.Env(), "TSFN may not have finalized.");
  };

  Napi::Value Call(const CallbackInfo &info) {
    _tsfn.NonBlockingCall();
    return info.Env().Undefined();
  };
};

} // namespace simple

Object InitThreadSafeFunctionExBasic(Env env) {

// A list of v4+ enabled spec namespaces.
#define V4_EXPORTS(V)                                                          \
  V(call)                                                                      \
  V(simple)                                                                    \
  V(existing)                                                                  \
  V(context)

// A list of v5+ enables spec namespaces.
#define V5_EXPORTS(V) V(empty)

#if NAPI_VERSION == 4
#define EXPORTS(V) V4_EXPORTS(V)
#else
#define EXPORTS(V)                                                             \
  V4_EXPORTS(V)                                                                \
  V5_EXPORTS(V)
#endif

  Object exports(Object::New(env));

#define V(modname) modname::TSFNWrap::Init(env, exports, #modname);
  EXPORTS(V)
#undef V

  return exports;
}

#endif
