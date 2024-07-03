#include <napi.h>

namespace {
class Test : public Napi::ObjectWrap<Test> {
 public:
  Test(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Test>(info) {
    syncFinalizerCalled = false;
    asyncFinalizerCalled = false;

    if (info.Length() > 0) {
      finalizeCb_ = Napi::Persistent(info[0].As<Napi::Function>());
    }
  }

  static void Initialize(Napi::Env env, Napi::Object exports) {
    exports.Set("Test",
                DefineClass(env,
                            "Test",
                            {
                                StaticAccessor("isSyncFinalizerCalled",
                                               &IsSyncFinalizerCalled,
                                               nullptr,
                                               napi_default),
                                StaticAccessor("isAsyncFinalizerCalled",
                                               &IsAsyncFinalizerCalled,
                                               nullptr,
                                               napi_default),
                            }));
  }

  void Finalize(Napi::BasicEnv /*env*/) { syncFinalizerCalled = true; }

  void Finalize(Napi::Env /*env*/) {
    asyncFinalizerCalled = true;
    if (!finalizeCb_.IsEmpty()) {
      finalizeCb_.Call({});
    }
  }

  static Napi::Value IsSyncFinalizerCalled(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), syncFinalizerCalled);
  }

  static Napi::Value IsAsyncFinalizerCalled(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), asyncFinalizerCalled);
  }

 private:
  Napi::FunctionReference finalizeCb_;

  static bool syncFinalizerCalled;
  static bool asyncFinalizerCalled;
};

bool Test::syncFinalizerCalled = false;
bool Test::asyncFinalizerCalled = false;

bool externalSyncFinalizerCalled = false;
bool externalAsyncFinalizerCalled = false;

Napi::Value CreateExternalSyncFinalizer(const Napi::CallbackInfo& info) {
  externalSyncFinalizerCalled = false;
  return Napi::External<int>::New(
      info.Env(), new int(1), [](Napi::BasicEnv /*env*/, int* data) {
        externalSyncFinalizerCalled = true;
        delete data;
      });
}

Napi::Value CreateExternalAsyncFinalizer(const Napi::CallbackInfo& info) {
  externalAsyncFinalizerCalled = false;
  return Napi::External<int>::New(
      info.Env(), new int(1), [](Napi::Env /*env*/, int* data) {
        externalAsyncFinalizerCalled = true;
        delete data;
      });
}

Napi::Value IsExternalSyncFinalizerCalled(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), externalSyncFinalizerCalled);
}

Napi::Value IsExternalAsyncFinalizerCalled(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), externalAsyncFinalizerCalled);
}

#ifdef NODE_API_EXPERIMENTAL_HAS_POST_FINALIZER
Napi::Value PostFinalizer(const Napi::CallbackInfo& info) {
  auto env = info.Env();

  env.PostFinalizer([callback = Napi::Persistent(info[0].As<Napi::Function>())](
                        Napi::Env /*env*/) { callback.Call({}); });

  return env.Undefined();
}

Napi::Value PostFinalizerWithData(const Napi::CallbackInfo& info) {
  auto env = info.Env();

  env.PostFinalizer(
      [callback = Napi::Persistent(info[0].As<Napi::Function>())](
          Napi::Env /*env*/, Napi::Reference<Napi::Value>* data) {
        callback.Call({data->Value()});
        delete data;
      },
      new Napi::Reference<Napi::Value>(Napi::Persistent(info[1])));

  return env.Undefined();
}

Napi::Value PostFinalizerWithDataAndHint(const Napi::CallbackInfo& info) {
  auto env = info.Env();

  env.PostFinalizer(
      [callback = Napi::Persistent(info[0].As<Napi::Function>())](
          Napi::Env /*env*/,
          Napi::Reference<Napi::Value>* data,
          Napi::Reference<Napi::Value>* hint) {
        callback.Call({data->Value(), hint->Value()});
        delete data;
        delete hint;
      },
      new Napi::Reference<Napi::Value>(Napi::Persistent(info[1])),
      new Napi::Reference<Napi::Value>(Napi::Persistent(info[2])));

  return env.Undefined();
}
#endif

}  // namespace

Napi::Object InitFinalizerOrder(Napi::Env env) {
  Napi::Object exports = Napi::Object::New(env);
  Test::Initialize(env, exports);
  exports["createExternalSyncFinalizer"] =
      Napi::Function::New(env, CreateExternalSyncFinalizer);
  exports["createExternalAsyncFinalizer"] =
      Napi::Function::New(env, CreateExternalAsyncFinalizer);
  exports["isExternalSyncFinalizerCalled"] =
      Napi::Function::New(env, IsExternalSyncFinalizerCalled);
  exports["isExternalAsyncFinalizerCalled"] =
      Napi::Function::New(env, IsExternalAsyncFinalizerCalled);

#ifdef NODE_API_EXPERIMENTAL_HAS_POST_FINALIZER
  exports["PostFinalizer"] = Napi::Function::New(env, PostFinalizer);
  exports["PostFinalizerWithData"] =
      Napi::Function::New(env, PostFinalizerWithData);
  exports["PostFinalizerWithDataAndHint"] =
      Napi::Function::New(env, PostFinalizerWithDataAndHint);
#endif
  return exports;
}
