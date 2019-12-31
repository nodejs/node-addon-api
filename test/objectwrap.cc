#include <napi.h>

Napi::ObjectReference testStaticContextRef;

Napi::Value StaticGetter(const Napi::CallbackInfo& /*info*/) {
  return testStaticContextRef.Value().Get("value");
}

void StaticSetter(const Napi::CallbackInfo& /*info*/, const Napi::Value& value) {
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

    if(info.Length() > 0) {
      finalizeCb_ = Napi::Persistent(info[0].As<Napi::Function>());
    }

  }

  void Setter(const Napi::CallbackInfo& /*info*/, const Napi::Value& value) {
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

  void TestVoidMethodT(const Napi::CallbackInfo &info) {
      value_ = info[0].ToString();
  }

  Napi::Value TestMethodT(const Napi::CallbackInfo &info) {
      return Napi::String::New(info.Env(), value_);
  }

  static Napi::Value TestStaticMethodT(const Napi::CallbackInfo& info) {
      return Napi::String::New(info.Env(), s_staticMethodText);
  }

  static void TestStaticVoidMethodT(const Napi::CallbackInfo& info) {
      s_staticMethodText = info[0].ToString();
  }

  static void Initialize(Napi::Env env, Napi::Object exports) {

    Napi::Symbol kTestStaticValueInternal = Napi::Symbol::New(env, "kTestStaticValueInternal");
    Napi::Symbol kTestStaticAccessorInternal = Napi::Symbol::New(env, "kTestStaticAccessorInternal");
    Napi::Symbol kTestStaticAccessorTInternal = Napi::Symbol::New(env, "kTestStaticAccessorTInternal");
    Napi::Symbol kTestStaticMethodInternal = Napi::Symbol::New(env, "kTestStaticMethodInternal");
    Napi::Symbol kTestStaticMethodTInternal     = Napi::Symbol::New(env, "kTestStaticMethodTInternal");
    Napi::Symbol kTestStaticVoidMethodTInternal = Napi::Symbol::New(env, "kTestStaticVoidMethodTInternal");

    Napi::Symbol kTestValueInternal = Napi::Symbol::New(env, "kTestValueInternal");
    Napi::Symbol kTestAccessorInternal = Napi::Symbol::New(env, "kTestAccessorInternal");
    Napi::Symbol kTestAccessorTInternal = Napi::Symbol::New(env, "kTestAccessorTInternal");
    Napi::Symbol kTestMethodInternal = Napi::Symbol::New(env, "kTestMethodInternal");
    Napi::Symbol kTestMethodTInternal     = Napi::Symbol::New(env, "kTestMethodTInternal");
    Napi::Symbol kTestVoidMethodTInternal = Napi::Symbol::New(env, "kTestVoidMethodTInternal");

    exports.Set("Test", DefineClass(env, "Test", {

      // expose symbols for testing
      StaticValue("kTestStaticValueInternal", kTestStaticValueInternal),
      StaticValue("kTestStaticAccessorInternal", kTestStaticAccessorInternal),
      StaticValue("kTestStaticAccessorTInternal", kTestStaticAccessorTInternal),
      StaticValue("kTestStaticMethodInternal", kTestStaticMethodInternal),
      StaticValue("kTestStaticMethodTInternal",     kTestStaticMethodTInternal),
      StaticValue("kTestStaticVoidMethodTInternal", kTestStaticVoidMethodTInternal),
      StaticValue("kTestValueInternal", kTestValueInternal),
      StaticValue("kTestAccessorInternal", kTestAccessorInternal),
      StaticValue("kTestAccessorTInternal", kTestAccessorTInternal),
      StaticValue("kTestMethodInternal", kTestMethodInternal),
      StaticValue("kTestMethodTInternal",     kTestMethodTInternal),
      StaticValue("kTestVoidMethodTInternal", kTestVoidMethodTInternal),

      // test data
      StaticValue("testStaticValue", Napi::String::New(env, "value"), napi_enumerable),
      StaticValue(kTestStaticValueInternal, Napi::Number::New(env, 5), napi_default),

      StaticAccessor("testStaticGetter", &StaticGetter, nullptr, napi_enumerable),
      StaticAccessor("testStaticSetter", nullptr, &StaticSetter, napi_default),
      StaticAccessor("testStaticGetSet", &StaticGetter, &StaticSetter, napi_enumerable),
      StaticAccessor(kTestStaticAccessorInternal, &StaticGetter, &StaticSetter, napi_enumerable),
      StaticAccessor<&StaticGetter>("testStaticGetterT"),
      StaticAccessor<&StaticGetter, &StaticSetter>("testStaticGetSetT"),
      StaticAccessor<&StaticGetter, &StaticSetter>(kTestStaticAccessorTInternal),

      StaticMethod("testStaticMethod", &TestStaticMethod, napi_enumerable),
      StaticMethod(kTestStaticMethodInternal, &TestStaticMethodInternal, napi_default),
      StaticMethod<&TestStaticVoidMethodT>("testStaticVoidMethodT"),
      StaticMethod<&TestStaticMethodT>("testStaticMethodT"),
      StaticMethod<&TestStaticVoidMethodT>(kTestStaticVoidMethodTInternal),
      StaticMethod<&TestStaticMethodT>(kTestStaticMethodTInternal),

      InstanceValue("testValue", Napi::Boolean::New(env, true), napi_enumerable),
      InstanceValue(kTestValueInternal, Napi::Boolean::New(env, false), napi_enumerable),

      InstanceAccessor("testGetter", &Test::Getter, nullptr, napi_enumerable),
      InstanceAccessor("testSetter", nullptr, &Test::Setter, napi_default),
      InstanceAccessor("testGetSet", &Test::Getter, &Test::Setter, napi_enumerable),
      InstanceAccessor(kTestAccessorInternal, &Test::Getter, &Test::Setter, napi_enumerable),
      InstanceAccessor<&Test::Getter>("testGetterT"),
      InstanceAccessor<&Test::Getter, &Test::Setter>("testGetSetT"),
      InstanceAccessor<&Test::Getter, &Test::Setter>(kTestAccessorInternal),

      InstanceMethod("testMethod", &Test::TestMethod, napi_enumerable),
      InstanceMethod(kTestMethodInternal, &Test::TestMethodInternal, napi_default),
      InstanceMethod<&Test::TestMethodT>("testMethodT"),
      InstanceMethod<&Test::TestVoidMethodT>("testVoidMethodT"),
      InstanceMethod<&Test::TestMethodT>(kTestMethodTInternal),
      InstanceMethod<&Test::TestVoidMethodT>(kTestVoidMethodTInternal),

      // conventions
      InstanceAccessor(Napi::Symbol::WellKnown(env, "toStringTag"), &Test::ToStringTag, nullptr, napi_enumerable),
      InstanceMethod(Napi::Symbol::WellKnown(env, "iterator"), &Test::Iterator, napi_default),

    }));
  }

  void Finalize(Napi::Env env) {

    if(finalizeCb_.IsEmpty()) {
      return;
    }

    finalizeCb_.Call(env.Global(), {Napi::Boolean::New(env, true)});
    finalizeCb_.Unref();

  }

private:
  std::string value_;
  Napi::FunctionReference finalizeCb_;

  static std::string s_staticMethodText;
};

std::string Test::s_staticMethodText;


class TestConstructorExceptions : public Napi::ObjectWrap<TestConstructorExceptions> {
public:
  TestConstructorExceptions(const Napi::CallbackInfo& info) : 
    Napi::ObjectWrap<TestConstructorExceptions>(info) {
      #ifdef NAPI_CPP_EXCEPTIONS
        throw Napi::Error::New(info.Env(), "Constructor exceptions should not cause v8 GC to fail");
      #else
        Napi::Error::New(info.Env(), "Constructor exceptions should not cause v8 GC to fail").ThrowAsJavaScriptException();
        return;
      #endif
    }

    static void Initialize(Napi::Env env, Napi::Object exports) {
      exports.Set("TestConstructorExceptions", DefineClass(env, "TestConstructorExceptions", {}));
    }
};



Napi::Object InitObjectWrap(Napi::Env env) {
  testStaticContextRef = Napi::Persistent(Napi::Object::New(env));
  testStaticContextRef.SuppressDestruct();

  Napi::Object exports = Napi::Object::New(env);
  Test::Initialize(env, exports);

#ifdef NAPI_CPP_EXCEPTIONS
  TestConstructorExceptions::Initialize(env, exports);
#endif
  return exports;
}
