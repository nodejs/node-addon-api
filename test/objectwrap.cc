#include <napi.h>

Napi::ObjectReference testStaticContextRef;

Napi::Value StaticGetter(const Napi::CallbackInfo& info) {
  return testStaticContextRef.Value().Get("value");
}

void StaticSetter(const Napi::CallbackInfo& info, const Napi::Value& value) {
  testStaticContextRef.Value().Set("value", value);
}

Napi::Value TestStaticMethod(const Napi::CallbackInfo& info) {
  std::string str = info[0].ToString();
  return Napi::String::New(info.Env(), str + " static");
}

Napi::Value TestStaticMethodInternal(const Napi::CallbackInfo& info) {
  std::string str = info[0].ToString();
  return Napi::String::New(info.Env(), str + " static internal");
}

class Test : public Napi::ObjectWrap<Test> {
public:
  Test(const Napi::CallbackInfo& info) :
    Napi::ObjectWrap<Test>(info) {
  }

  void Setter(const Napi::CallbackInfo& info, const Napi::Value& value) {
    value_ = value.ToString();
  }

  Napi::Value Getter(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), value_);
  }

  Napi::Value TestMethod(const Napi::CallbackInfo& info) {
    std::string str = info[0].ToString();
    return Napi::String::New(info.Env(), str + " instance");
  }

  Napi::Value TestMethodInternal(const Napi::CallbackInfo& info) {
    std::string str = info[0].ToString();
    return Napi::String::New(info.Env(), str + " instance internal");
  }

  Napi::Value ToStringTag(const Napi::CallbackInfo& info) {
    return Napi::String::From(info.Env(), "TestTag");
  }

  // creates dummy array, returns `([value])[Symbol.iterator]()`
  Napi::Value Iterator(const Napi::CallbackInfo& info) {
    Napi::Array array = Napi::Array::New(info.Env());
    array.Set(array.Length(), Napi::String::From(info.Env(), value_));
    return array.Get(Napi::Symbol::WellKnown(info.Env(), "iterator")).As<Napi::Function>().Call(array, {});
  }

  static void Initialize(Napi::Env env, Napi::Object exports) {
    
    Napi::Symbol kTestStaticValueInternal = Napi::Symbol::New(env, "kTestStaticValueInternal");
    Napi::Symbol kTestStaticAccessorInternal = Napi::Symbol::New(env, "kTestStaticAccessorInternal");
    Napi::Symbol kTestStaticMethodInternal = Napi::Symbol::New(env, "kTestStaticMethodInternal");

    Napi::Symbol kTestValueInternal = Napi::Symbol::New(env, "kTestValueInternal");
    Napi::Symbol kTestAccessorInternal = Napi::Symbol::New(env, "kTestAccessorInternal");
    Napi::Symbol kTestMethodInternal = Napi::Symbol::New(env, "kTestMethodInternal");
    
    exports.Set("Test", DefineClass(env, "Test", {

      // expose symbols for testing
      StaticValue("kTestStaticValueInternal", kTestStaticValueInternal),
      StaticValue("kTestStaticAccessorInternal", kTestStaticAccessorInternal),
      StaticValue("kTestStaticMethodInternal", kTestStaticMethodInternal),
      StaticValue("kTestValueInternal", kTestValueInternal),
      StaticValue("kTestAccessorInternal", kTestAccessorInternal),
      StaticValue("kTestMethodInternal", kTestMethodInternal),

      // test data
      StaticValue("testStaticValue", Napi::String::New(env, "value"), napi_enumerable),
      StaticValue(kTestStaticValueInternal, Napi::Number::New(env, 5), napi_default),

      StaticAccessor("testStaticGetter", &StaticGetter, nullptr, napi_enumerable),
      StaticAccessor("testStaticSetter", nullptr, &StaticSetter, napi_default),
      StaticAccessor("testStaticGetSet", &StaticGetter, &StaticSetter, napi_enumerable),
      StaticAccessor(kTestStaticAccessorInternal, &StaticGetter, &StaticSetter, napi_enumerable),

      StaticMethod("testStaticMethod", &TestStaticMethod, napi_enumerable),
      StaticMethod(kTestStaticMethodInternal, &TestStaticMethodInternal, napi_default),

      InstanceValue("testValue", Napi::Boolean::New(env, true), napi_enumerable),
      InstanceValue(kTestValueInternal, Napi::Boolean::New(env, false), napi_enumerable),

      InstanceAccessor("testGetter", &Test::Getter, nullptr, napi_enumerable),
      InstanceAccessor("testSetter", nullptr, &Test::Setter, napi_default),
      InstanceAccessor("testGetSet", &Test::Getter, &Test::Setter, napi_enumerable),
      InstanceAccessor(kTestAccessorInternal, &Test::Getter, &Test::Setter, napi_enumerable),

      InstanceMethod("testMethod", &Test::TestMethod, napi_enumerable),
      InstanceMethod(kTestMethodInternal, &Test::TestMethodInternal, napi_default),

      // conventions
      InstanceAccessor(Napi::Symbol::WellKnown(env, "toStringTag"), &Test::ToStringTag, nullptr, napi_enumerable),
      InstanceMethod(Napi::Symbol::WellKnown(env, "iterator"), &Test::Iterator, napi_default),

    }));
  }

private:
  std::string value_;
};

Napi::Object InitObjectWrap(Napi::Env env) {
  testStaticContextRef = Napi::Persistent(Napi::Object::New(env));
  testStaticContextRef.SuppressDestruct();

  Napi::Object exports = Napi::Object::New(env);
  Test::Initialize(env, exports);
  return exports;
}
