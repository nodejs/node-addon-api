#include "napi.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace call {

// Context of our TSFN.
using Context = std::nullptr_t;

// Data passed (as pointer) to ThreadSafeFunctionEx::[Non]BlockingCall
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

// Full type of our ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<Context, Data, CallJs>;

// A JS-accessible wrap that holds a TSFN.
class TSFNWrap : public ObjectWrap<TSFNWrap> {
public:
  static void Init(Napi::Env env, Object exports, const std::string &ns) {
    Function func =
        DefineClass(env, "TSFNCall",
                    {InstanceMethod("call", &TSFNWrap::Call),
                     InstanceMethod("release", &TSFNWrap::Release)});

    auto locals(Object::New(env));
    exports.Set(ns, locals);
    locals.Set("TSFNWrap", func);
  }

  TSFNWrap(const CallbackInfo &info) : ObjectWrap<TSFNWrap>(info) {
    Napi::Env env = info.Env();
    Function callback = info[0].As<Function>();
    _tsfn = TSFN::New(env,      // napi_env env,
                      callback, // const Function& callback,
                      "Test",   // ResourceString resourceName,
                      0,        // size_t maxQueueSize,
                      1,        // size_t initialThreadCount,
                      nullptr,
                      [this](Napi::Env env, void *,
                             Context *ctx) { // Finalizer finalizeCallback,
                        if (_deferred) {
                          _deferred->Resolve(Boolean::New(env, true));
                          _deferred.release();
                        }
                      });
  }

  Napi::Value Call(const CallbackInfo &info) {
    Napi::Env env = info.Env();
    Data *data = new Data{Napi::Reference<Napi::Value>(Persistent(info[0])),
                          Promise::Deferred::New(env)};
    _tsfn.NonBlockingCall(data);
    return data->deferred.Promise();
  };

  Napi::Value Release(const CallbackInfo &info) {
    if (_deferred) {
      return _deferred->Promise();
    }

    auto env = info.Env();
    _deferred.reset(new Promise::Deferred(Promise::Deferred::New(env)));
    _tsfn.Release();
    return _deferred->Promise();
  };

private:
  TSFN _tsfn;
  std::unique_ptr<Promise::Deferred> _deferred;
};

} // namespace call

namespace context {

// Context of our TSFN.
using Context = Reference<Napi::Value>;

// Data passed (as pointer) to ThreadSafeFunctionEx::[Non]BlockingCall
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

// Full type of our ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<Context, Data, CallJs>;

// A JS-accessible wrap that holds a TSFN.
class TSFNWrap : public ObjectWrap<TSFNWrap> {
public:
  static void Init(Napi::Env env, Object exports, const char *ns) {
    Function func = DefineClass(
        env, "TSFNWrap",
        {InstanceMethod("getContextByCall", &TSFNWrap::GetContextByCall),
         InstanceMethod("getContextFromTsfn", &TSFNWrap::GetContextFromTsfn),
         InstanceMethod("release", &TSFNWrap::Release)});

    auto locals(Object::New(env));
    exports.Set(ns, locals);
    locals.Set("TSFNWrap", func);
  }

  TSFNWrap(const CallbackInfo &info) : ObjectWrap<TSFNWrap>(info) {
    Napi::Env env = info.Env();

    Context *context = new Context(Persistent(info[0]));

    _tsfn = TSFN::New(info.Env(), // napi_env env,
                      Function::New(env,
                                    [](const CallbackInfo & /*info*/) {
                                    }), // const Function& callback,
                      Value(),          // const Object& resource,
                      "Test",           // ResourceString resourceName,
                      0,                // size_t maxQueueSize,
                      1,                // size_t initialThreadCount,
                      context,          // Context* context,
                      [this](Napi::Env env, void *,
                             Context *ctx) { // Finalizer finalizeCallback,
                        if (_deferred) {
                          _deferred->Resolve(Boolean::New(env, true));
                          _deferred.release();
                        }
                      });
  }

  Napi::Value GetContextByCall(const CallbackInfo &info) {
    Napi::Env env = info.Env();
    auto *callData = new Data(env);
    _tsfn.NonBlockingCall(callData);
    return callData->Promise();
  };

  Napi::Value GetContextFromTsfn(const CallbackInfo &) {
    return _tsfn.GetContext()->Value();
  };

  Napi::Value Release(const CallbackInfo &info) {
    if (_deferred) {
      return _deferred->Promise();
    }
    auto env = info.Env();
    _deferred.reset(new Promise::Deferred(Promise::Deferred::New(env)));
    _tsfn.Release();
    return _deferred->Promise();
  };

private:
  TSFN _tsfn;
  std::unique_ptr<Promise::Deferred> _deferred;
};
} // namespace context

namespace empty {
#if NAPI_VERSION > 4

using Context = void;

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

// Full type of our ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<void, Data, CallJs>;

// A JS-accessible wrap that holds a TSFN.
class TSFNWrap : public ObjectWrap<TSFNWrap> {
public:
  static void Init(Napi::Env env, Object exports, const std::string &ns) {
    Function func =
        DefineClass(env, "TSFNWrap",
                    {InstanceMethod("call", &TSFNWrap::Call),
                     InstanceMethod("release", &TSFNWrap::Release)});

    auto locals(Object::New(env));
    exports.Set(ns, locals);
    locals.Set("TSFNWrap", func);
  }

  TSFNWrap(const CallbackInfo &info) : ObjectWrap<TSFNWrap>(info) {

    auto env = info.Env();
    _tsfn = TSFN::New(env,    // napi_env env,
                      "Test", // ResourceString resourceName,
                      0,      // size_t maxQueueSize,
                      1,      // size_t initialThreadCount,
                      nullptr,
                      [this](Napi::Env env, void *,
                             Context *ctx) { // Finalizer finalizeCallback,
                        if (_deferred) {
                          _deferred->Resolve(Boolean::New(env, true));
                          _deferred.release();
                        }
                      });
  }

  Napi::Value Release(const CallbackInfo &info) {
    // Since this is actually a SINGLE-THREADED test, we don't have to worry
    // about race conditions on accessing `_deferred`.
    if (_deferred) {
      return _deferred->Promise();
    }

    auto env = info.Env();
    _deferred.reset(new Promise::Deferred(Promise::Deferred::New(env)));
    _tsfn.Release();
    return _deferred->Promise();
  };

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

private:
  TSFN _tsfn;
  std::unique_ptr<Promise::Deferred> _deferred;
};

#endif
} // namespace empty

namespace existing {

struct Data {
  Promise::Deferred deferred;
  bool reject;
};

// CallJs callback function provided to `napi_create_threadsafe_function`. It is
// _NOT_ used by `Napi::ThreadSafeFunctionEx<>`.
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
// parameter is the `TSFNWrap` object itself. We forward-declare, so we can use
// it as an argument inside `ThreadSafeFunctionEx<>`. This also allows us to
// statically get the correct type when using `tsfn.GetContext()`. The converse
// is true: if the Context type does _not_ match that provided to the underlying
// napi_create_threadsafe_function, then the static type will be incorrect.
class TSFNWrap;

// Full type of our ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<TSFNWrap, Data>;

// A JS-accessible wrap that holds a TSFN.
class TSFNWrap : public ObjectWrap<TSFNWrap> {
public:
  static void Init(Napi::Env env, Object exports, const std::string &ns) {
    Function func =
        DefineClass(env, "TSFNWrap",
                    {InstanceMethod("call", &TSFNWrap::Call),
                     InstanceMethod("release", &TSFNWrap::Release)});
    auto locals(Object::New(env));
    exports.Set(ns, locals);
    locals.Set("TSFNWrap", func);
  }

  TSFNWrap(const CallbackInfo &info) : ObjectWrap<TSFNWrap>(info) {

    auto env = info.Env();
#if NAPI_VERSION == 4
    napi_threadsafe_function napi_tsfn;
    auto status = napi_create_threadsafe_function(
        info.Env(), TSFN::DefaultFunctionFactory(env), nullptr,
        String::From(info.Env(), "Test"), 0, 1, nullptr, nullptr, nullptr,
        CallJs, &napi_tsfn);
    if (status != napi_ok) {
      NAPI_THROW_IF_FAILED(env, status);
    }
    // A threadsafe function on N-API 4 still requires a callback function.
    _tsfn = TSFN(napi_tsfn);
#else
    napi_threadsafe_function napi_tsfn;
    auto status = napi_create_threadsafe_function(
        info.Env(), nullptr, nullptr, String::From(info.Env(), "Test"), 0, 1,
        nullptr, Finalizer, this, CallJs, &napi_tsfn);
    if (status != napi_ok) {
      NAPI_THROW_IF_FAILED(env, status);
    }
    _tsfn = TSFN(napi_tsfn);
#endif
  }

  Napi::Value Release(const CallbackInfo &info) {
    if (_deferred) {
      return _deferred->Promise();
    }
    auto env = info.Env();
    _deferred.reset(new Promise::Deferred(Promise::Deferred::New(env)));
    _tsfn.Release();
    return _deferred->Promise();
  };

  Napi::Value Call(const CallbackInfo &info) {
    auto *data =
        new Data{Promise::Deferred::New(info.Env()), info[0].ToBoolean()};
    _tsfn.NonBlockingCall(data);
    return data->deferred.Promise();
  };

private:
  TSFN _tsfn;
  std::unique_ptr<Promise::Deferred> _deferred;

  static void Finalizer(napi_env env, void * /*data*/, void *ctx) {
    TSFNWrap *tsfn = static_cast<TSFNWrap *>(ctx);
    tsfn->Finalizer(env);
  }

  void Finalizer(napi_env e) {
    if (_deferred) {
      _deferred->Resolve(Boolean::New(e, true));
      _deferred.release();
    }
    // Finalizer finalizeCallback,
  }
};

} // namespace existing
namespace simple {

// Full type of our ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<>;

// A JS-accessible wrap that holds a TSFN.
class TSFNWrap : public ObjectWrap<TSFNWrap> {
public:
  static void Init(Napi::Env env, Object exports, const std::string &ns) {
    Function func =
        DefineClass(env, "TSFNSimple",
                    {InstanceMethod("call", &TSFNWrap::Call),
                     InstanceMethod("release", &TSFNWrap::Release)});

    auto locals(Object::New(env));
    exports.Set(ns, locals);
    locals.Set("TSFNWrap", func);
  }

  TSFNWrap(const CallbackInfo &info) : ObjectWrap<TSFNWrap>(info) {

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

private:
  TSFN _tsfn;
};

} // namespace simple

Object InitThreadSafeFunctionExBasic(Env env) {

// A list of v4+ enables spec namespaces.
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
