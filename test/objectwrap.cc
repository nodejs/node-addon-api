#include <napi.h>

class TestIter : public Napi::ObjectWrap<TestIter> {
public:
  TestIter(const Napi::CallbackInfo& info) : Napi::ObjectWrap<TestIter>(info) {}

  Napi::Value Next(const Napi::CallbackInfo& info) {
    auto object = Napi::Object::New(info.Env());
    object.Set("done", Napi::Boolean::New(info.Env(), true));
    return object;
  }

  static Napi::FunctionReference Initialize(Napi::Env env) {
    return Napi::Persistent(DefineClass(env, "TestIter", {
      InstanceMethod("next", &TestIter::Next),
    }));
  }
};

class Test : public Napi::ObjectWrap<Test> {
public:
  Test(const Napi::CallbackInfo& info) :
    Napi::ObjectWrap<Test>(info),
    Constructor(TestIter::Initialize(info.Env())) {
  }

  void SetMethod(const Napi::CallbackInfo& info) {
    value = info[0].As<Napi::Number>();
  }

  Napi::Value GetMethod(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), value);
  }

  Napi::Value Iter(const Napi::CallbackInfo& info) {
    return Constructor.New({});
  }

  void Setter(const Napi::CallbackInfo& info, const Napi::Value& new_value) {
    value = new_value.As<Napi::Number>();
  }

  Napi::Value Getter(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), value);
  }

  static void Initialize(Napi::Env env, Napi::Object exports) {
    exports.Set("Test", DefineClass(env, "Test", {
      InstanceMethod("test_set_method", &Test::SetMethod),
      InstanceMethod("test_get_method", &Test::GetMethod),
      InstanceMethod(Napi::Symbol::WellKnown(env, "iterator"), &Test::Iter),
      InstanceAccessor("test_getter_only", &Test::Getter, nullptr),
      InstanceAccessor("test_setter_only", nullptr, &Test::Setter),
      InstanceAccessor("test_getter_setter", &Test::Getter, &Test::Setter),
    }));
  }

private:
  uint32_t value;
  Napi::FunctionReference Constructor;
};

Napi::Object InitObjectWrap(Napi::Env env) {
  Napi::Object exports = Napi::Object::New(env);
  Test::Initialize(env, exports);
  return exports;
}
