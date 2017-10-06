#include <napi.h>

class TestIter : public Napi::ObjectWrap<TestIter> {
public:
  TestIter(const Napi::CallbackInfo& info) : Napi::ObjectWrap<TestIter>(info) {}

  Napi::Value Next(const Napi::CallbackInfo& info) {
    auto object = Napi::Object::New(info.Env());
    object.Set("done", Napi::Boolean::New(info.Env(), true));
    return object;
  }

  static void Initialize(Napi::Env env, Napi::Object exports) {
    Constructor = Napi::Persistent(DefineClass(env, "TestIter", {
      InstanceMethod("next", &TestIter::Next),
    }));
  }

  static Napi::FunctionReference Constructor;
};

Napi::FunctionReference TestIter::Constructor;

class Test : public Napi::ObjectWrap<Test> {
public:
  Test(const Napi::CallbackInfo& info) :
    Napi::ObjectWrap<Test>(info) {
  }

  void Set(const Napi::CallbackInfo& info) {
    value = info[0].As<Napi::Number>();
  }

  Napi::Value Get(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), value);
  }

  Napi::Value Iter(const Napi::CallbackInfo& info) {
    return TestIter::Constructor.New({});
  }

  static void Initialize(Napi::Env env, Napi::Object exports) {
    exports.Set("Test", DefineClass(env, "Test", {
      InstanceMethod("test_set", &Test::Set),
      InstanceMethod("test_get", &Test::Get),
      InstanceMethod(Napi::Symbol::WellKnown(env, "iterator"), &Test::Iter)
    }));
  }

private:
  uint32_t value;
};

Napi::Object InitObjectWrap(Napi::Env env) {
  Napi::Object exports = Napi::Object::New(env);
  Test::Initialize(env, exports);
  TestIter::Initialize(env, exports);
  return exports;
}
