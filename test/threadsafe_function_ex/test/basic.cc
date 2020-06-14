#include <array>
#include "napi.h"
#include "../util/util.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace call {

// Context of the TSFN.
using Context = std::nullptr_t;

// Data passed (as pointer) to [Non]BlockingCall
struct Data {
  Reference<Napi::Value> data;
  Promise::Deferred deferred;
};

// CallJs callback function
static void CallJs(Napi::Env env, Napi::Function jsCallback,
                   Context * /*context*/, Data *data) {
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
using TSFN = ThreadSafeFunctionEx<Context, Data, CallJs>;

class TSFNWrap;
using base = tsfnutil::TSFNWrapBase<TSFNWrap, Context, TSFN>;

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
                      &_deferred              // FinalizerDataType data
    );
  }

  static std::array<ClassPropertyDescriptor<TSFNWrap>, 2> InstanceMethods() {
    return {InstanceMethod("release", &TSFNWrap::Release),
            InstanceMethod("call", &TSFNWrap::Call)};
  }

  Napi::Value Call(const CallbackInfo &info) {
    Napi::Env env = info.Env();
    Data *data = new Data{Napi::Reference<Napi::Value>(Persistent(info[0])),
                          Promise::Deferred::New(env)};
    _tsfn.NonBlockingCall(data);
    return data->deferred.Promise();
  };

};

} // namespace call

namespace context {

// Context of the TSFN.
using Context = Reference<Napi::Value>;

// Data passed (as pointer) to [Non]BlockingCall
using Data = Promise::Deferred;

// CallJs callback function
static void CallJs(Napi::Env env, Napi::Function /*jsCallback*/,
                   Context *context, Data *data) {
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
using TSFN = ThreadSafeFunctionEx<Context, Data, CallJs>;

class TSFNWrap;
using base = tsfnutil::TSFNWrapBase<TSFNWrap, Context, TSFN>;

// A JS-accessible wrap that holds the TSFN.
class TSFNWrap : public base {
public:
  TSFNWrap(const CallbackInfo &info) : base(info) {
    Napi::Env env = info.Env();

    Context *context = new Context(Persistent(info[0]));

    _tsfn = TSFN::New(
        env,                               // napi_env env,
        TSFN::DefaultFunctionFactory(env), // const Function& callback,
        Value(),                           // const Object& resource,
        "Test",                            // ResourceString resourceName,
        0,                                 // size_t maxQueueSize,
        1,                                 // size_t initialThreadCount,
        context,                           // Context* context,
        base::Finalizer,                   // Finalizer finalizer
        &_deferred                         // FinalizerDataType data
    );
  }

  static std::array<ClassPropertyDescriptor<TSFNWrap>, 3> InstanceMethods() {
    return {InstanceMethod("call", &TSFNWrap::Call),
            InstanceMethod("getContext", &TSFNWrap::GetContext),
            InstanceMethod("release", &TSFNWrap::Release)};
  }

  Napi::Value Call(const CallbackInfo &info) {
    auto *callData = new Data(info.Env());
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
using Context = std::nullptr_t;

// Data passed (as pointer) to [Non]BlockingCall
struct Data {
  Promise::Deferred deferred;
  bool reject;
};

// CallJs callback function
static void CallJs(Napi::Env env, Function jsCallback, Context * /*context*/,
                   Data *data) {
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

// Full type of the ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<Context, Data, CallJs>;

class TSFNWrap;
using base = tsfnutil::TSFNWrapBase<TSFNWrap, Context, TSFN>;

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
                      &_deferred       // FinalizerDataType data
    );
  }

  static std::array<ClassPropertyDescriptor<TSFNWrap>, 2> InstanceMethods() {
    return {InstanceMethod("release", &TSFNWrap::Release),
            InstanceMethod("call", &TSFNWrap::Call)};
  }

  Napi::Value Call(const CallbackInfo &info) {
    if (info.Length() == 0 || !info[0].IsBoolean()) {
      NAPI_THROW(
          Napi::TypeError::New(info.Env(), "Expected argument 0 to be boolean"),
          Value());
    }

    auto *data =
        new Data{Promise::Deferred::New(info.Env()), info[0].ToBoolean()};
    _tsfn.NonBlockingCall(data);
    return data->deferred.Promise();
  };
};

#endif
} // namespace empty

namespace existing {

// Data passed (as pointer) to [Non]BlockingCall
struct Data {
  Promise::Deferred deferred;
  bool reject;
};

// CallJs callback function provided to `napi_create_threadsafe_function`. It is
// _NOT_ used by `Napi::ThreadSafeFunctionEx<>`, which is why these arguments
// are napi_*.
static void CallJs(napi_env env, napi_value jsCallback, void * /*context*/,
                   void *data) {
  Data *casted = static_cast<Data *>(data);
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

// This test creates a native napi_threadsafe_function itself, whose `context`
// parameter is the `TSFNWrap` object. We forward-declare, so we can use
// it as an argument inside `ThreadSafeFunctionEx<>`. This also allows us to
// statically get the correct type when using `tsfn.GetContext()`. The converse
// is true: if the Context type does _not_ match that provided to the underlying
// napi_create_threadsafe_function, then the static type will be incorrect.
class TSFNWrap;

// Context of the TSFN.
using Context = TSFNWrap;

// Full type of our ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<Context, Data>;
using base = tsfnutil::TSFNWrapBase<TSFNWrap, Context, TSFN>;

// A JS-accessible wrap that holds a TSFN.
class TSFNWrap : public base {
public:
  TSFNWrap(const CallbackInfo &info) : base(info) {

    auto env = info.Env();
#if NAPI_VERSION == 4
    napi_threadsafe_function napi_tsfn;

    // A threadsafe function on N-API 4 still requires a callback function, so
    // this uses the `DefaultFunctionFactory` helper method to return a no-op
    // Function.
    auto status = napi_create_threadsafe_function(
        info.Env(), TSFN::DefaultFunctionFactory(env), nullptr,
        String::From(info.Env(), "Test"), 0, 1, nullptr, nullptr, nullptr,
        CallJs, &napi_tsfn);
    if (status != napi_ok) {
      NAPI_THROW_IF_FAILED(env, status);
    }
    _tsfn = TSFN(napi_tsfn);
#else
    napi_threadsafe_function napi_tsfn;

    // A threadsafe function may be `nullptr` on N-API 5+ as long as a `CallJS`
    // is present.
    auto status = napi_create_threadsafe_function(
        info.Env(), nullptr, nullptr, String::From(info.Env(), "Test"), 0, 1,
        nullptr, Finalizer, this, CallJs, &napi_tsfn);
    if (status != napi_ok) {
      NAPI_THROW_IF_FAILED(env, status);
    }
    _tsfn = TSFN(napi_tsfn);
#endif
  }

  static std::array<ClassPropertyDescriptor<TSFNWrap>, 2> InstanceMethods() {
    return {InstanceMethod("release", &TSFNWrap::Release),
            InstanceMethod("call", &TSFNWrap::Call)};
  }

  Napi::Value Call(const CallbackInfo &info) {
    auto *data =
        new Data{Promise::Deferred::New(info.Env()), info[0].ToBoolean()};
    _tsfn.NonBlockingCall(data);
    return data->deferred.Promise();
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

using Context = std::nullptr_t;

// Full type of our ThreadSafeFunctionEx. We don't specify the `Context` here
// (even though the _default_ for the type argument is `std::nullptr_t`) to
// demonstrate construction with no type arguments.
using TSFN = ThreadSafeFunctionEx<>;

class TSFNWrap;
using base = tsfnutil::TSFNWrapBase<TSFNWrap, Context, TSFN>;

// A JS-accessible wrap that holds a TSFN.
class TSFNWrap : public base {
public:
  TSFNWrap(const CallbackInfo &info) : base(info) {

    auto env = info.Env();
#if NAPI_VERSION == 4
    // A threadsafe function on N-API 4 still requires a callback function.
    _tsfn = TSFN::New(
        env, // napi_env env,
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

  static std::array<ClassPropertyDescriptor<TSFNWrap>, 2> InstanceMethods() {
    return {InstanceMethod("release", &TSFNWrap::Release),
            InstanceMethod("call", &TSFNWrap::Call)};
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
