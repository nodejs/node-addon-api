#include <napi.h>

namespace {
class Test : public Napi::ObjectWrap<Test> {
 public:
  Test(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Test>(info) {
    nogcFinalizerCalled = false;
    gcFinalizerCalled = false;

    if (info.Length() > 0) {
      finalizeCb_ = Napi::Persistent(info[0].As<Napi::Function>());
    }
  }

  static void Initialize(Napi::Env env, Napi::Object exports) {
    exports.Set("Test",
                DefineClass(env,
                            "Test",
                            {
                                StaticAccessor("isNogcFinalizerCalled",
                                               &IsNogcFinalizerCalled,
                                               nullptr,
                                               napi_default),
                                StaticAccessor("isGcFinalizerCalled",
                                               &IsGcFinalizerCalled,
                                               nullptr,
                                               napi_default),
                            }));
  }

  void Finalize(Napi::NogcEnv /*env*/) { nogcFinalizerCalled = true; }

  void Finalize(Napi::Env /*env*/) {
    gcFinalizerCalled = true;
    if (!finalizeCb_.IsEmpty()) {
      finalizeCb_.Call({});
    }
  }

  static Napi::Value IsNogcFinalizerCalled(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), nogcFinalizerCalled);
  }

  static Napi::Value IsGcFinalizerCalled(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), gcFinalizerCalled);
  }

 private:
  Napi::FunctionReference finalizeCb_;

  static bool nogcFinalizerCalled;
  static bool gcFinalizerCalled;
};

bool Test::nogcFinalizerCalled = false;
bool Test::gcFinalizerCalled = false;

bool externalNogcFinalizerCalled = false;
bool externalGcFinalizerCalled = false;

Napi::Value CreateExternalNogc(const Napi::CallbackInfo& info) {
  externalNogcFinalizerCalled = false;
  return Napi::External<int>::New(
      info.Env(), new int(1), [](Napi::NogcEnv /*env*/, int* data) {
        externalNogcFinalizerCalled = true;
        delete data;
      });
}

Napi::Value CreateExternalGc(const Napi::CallbackInfo& info) {
  externalGcFinalizerCalled = false;
  return Napi::External<int>::New(
      info.Env(), new int(1), [](Napi::Env /*env*/, int* data) {
        externalGcFinalizerCalled = true;
        delete data;
      });
}

Napi::Value IsExternalNogcFinalizerCalled(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), externalNogcFinalizerCalled);
}

Napi::Value IsExternalGcFinalizerCalled(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), externalGcFinalizerCalled);
}

#ifdef NODE_API_EXPERIMENTAL_HAS_POST_FINALIZER
Napi::Value AddPostFinalizer(const Napi::CallbackInfo& info) {
  auto env = info.Env();

  env.AddPostFinalizer(
      [callback = Napi::Persistent(info[0].As<Napi::Function>())](
          Napi::Env /*env*/) { callback.Call({}); });

  return env.Undefined();
}

Napi::Value AddPostFinalizerWithData(const Napi::CallbackInfo& info) {
  auto env = info.Env();

  env.AddPostFinalizer(
      [callback = Napi::Persistent(info[0].As<Napi::Function>())](
          Napi::Env /*env*/, Napi::Reference<Napi::Value>* data) {
        callback.Call({data->Value()});
        delete data;
      },
      new Napi::Reference<Napi::Value>(Napi::Persistent(info[1])));
  return env.Undefined();
}

Napi::Value AddPostFinalizerWithDataAndHint(const Napi::CallbackInfo& info) {
  auto env = info.Env();

  env.AddPostFinalizer(
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
  exports["CreateExternalNogc"] = Napi::Function::New(env, CreateExternalNogc);
  exports["CreateExternalGc"] = Napi::Function::New(env, CreateExternalGc);
  exports["isExternalNogcFinalizerCalled"] =
      Napi::Function::New(env, IsExternalNogcFinalizerCalled);
  exports["isExternalGcFinalizerCalled"] =
      Napi::Function::New(env, IsExternalGcFinalizerCalled);

#ifdef NODE_API_EXPERIMENTAL_HAS_POST_FINALIZER
  exports["AddPostFinalizer"] = Napi::Function::New(env, AddPostFinalizer);
  exports["AddPostFinalizerWithData"] =
      Napi::Function::New(env, AddPostFinalizerWithData);
  exports["AddPostFinalizerWithDataAndHint"] =
      Napi::Function::New(env, AddPostFinalizerWithDataAndHint);
#endif
  return exports;
}
