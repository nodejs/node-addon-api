#include <napi.h>

namespace {
class Test : public Napi::ObjectWrap<Test> {
 public:
  Test(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Test>(info) {
    basicFinalizerCalled = false;
    finalizerCalled = false;

    if (info.Length() > 0) {
      finalizeCb_ = Napi::Persistent(info[0].As<Napi::Function>());
    }
  }

  static void Initialize(Napi::Env env, Napi::Object exports) {
    exports.Set("Test",
                DefineClass(env,
                            "Test",
                            {
                                StaticAccessor("isBasicFinalizerCalled",
                                               &IsBasicFinalizerCalled,
                                               nullptr,
                                               napi_default),
                                StaticAccessor("isFinalizerCalled",
                                               &IsFinalizerCalled,
                                               nullptr,
                                               napi_default),
                            }));
  }

  void Finalize(Napi::BasicEnv /*env*/) { basicFinalizerCalled = true; }

  void Finalize(Napi::Env /*env*/) {
    finalizerCalled = true;
    if (!finalizeCb_.IsEmpty()) {
      finalizeCb_.Call({});
    }
  }

  static Napi::Value IsBasicFinalizerCalled(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), basicFinalizerCalled);
  }

  static Napi::Value IsFinalizerCalled(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), finalizerCalled);
  }

 private:
  Napi::FunctionReference finalizeCb_;

  static bool basicFinalizerCalled;
  static bool finalizerCalled;
};

bool Test::basicFinalizerCalled = false;
bool Test::finalizerCalled = false;

bool externalBasicFinalizerCalled = false;
bool externalFinalizerCalled = false;

Napi::Value CreateExternalBasicFinalizer(const Napi::CallbackInfo& info) {
  externalBasicFinalizerCalled = false;
  return Napi::External<int>::New(
      info.Env(), new int(1), [](Napi::BasicEnv /*env*/, int* data) {
        externalBasicFinalizerCalled = true;
        delete data;
      });
}

Napi::Value CreateExternalFinalizer(const Napi::CallbackInfo& info) {
  externalFinalizerCalled = false;
  return Napi::External<int>::New(
      info.Env(), new int(1), [](Napi::Env /*env*/, int* data) {
        externalFinalizerCalled = true;
        delete data;
      });
}

Napi::Value isExternalBasicFinalizerCalled(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), externalBasicFinalizerCalled);
}

Napi::Value IsExternalFinalizerCalled(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), externalFinalizerCalled);
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
  exports["createExternalBasicFinalizer"] =
      Napi::Function::New(env, CreateExternalBasicFinalizer);
  exports["createExternalFinalizer"] =
      Napi::Function::New(env, CreateExternalFinalizer);
  exports["isExternalBasicFinalizerCalled"] =
      Napi::Function::New(env, isExternalBasicFinalizerCalled);
  exports["isExternalFinalizerCalled"] =
      Napi::Function::New(env, IsExternalFinalizerCalled);

#ifdef NODE_API_EXPERIMENTAL_HAS_POST_FINALIZER
  exports["PostFinalizer"] = Napi::Function::New(env, PostFinalizer);
  exports["PostFinalizerWithData"] =
      Napi::Function::New(env, PostFinalizerWithData);
  exports["PostFinalizerWithDataAndHint"] =
      Napi::Function::New(env, PostFinalizerWithDataAndHint);
#endif
  return exports;
}
