#define NAPI_UNSAFER_UNWRAP
#include <napi.h>

class TestSub {
public:
  virtual ~TestSub() {}
  virtual int Get1() { return 1; }
};

class Test : public Napi::ObjectWrap<Test>, public TestSub {
public:
  Test(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Test>(info) {

  }

  static void Initialize(Napi::Env env, Napi::Object exports) {
    exports.Set("Test", DefineClass(env, "Test", {}));
  }
};

class TestReceiver : public Napi::ObjectWrap<TestReceiver> {
public:
  TestReceiver(const Napi::CallbackInfo& info) : Napi::ObjectWrap<TestReceiver>(info) {

  }

  Napi::Value Unwrap(const Napi::CallbackInfo& info) {
    
    // This is okay => reinterpret_cast of the same type is valid
    int n = Napi::ObjectWrap<Test>::Unwrap(info[0].ToObject())->Get1();

    // This should fail => reinterpret_cast of a subclass (or any other type) is possible but NOT allowed
    // If you're lucky you get a segfault. If not, you may get a stacktrace invoking `->A()`, but invoked `->B()` or any other method.
    n = Napi::ObjectWrap<TestSub>::Unwrap(info[0].ToObject())->Get1();

    // => do we have "should not compile" tests?
    // without the fix, this will compile and you'll spend lot of time debugging :(

    return Napi::Number::New(info.Env(), n);
  }

  static void Initialize(Napi::Env env, Napi::Object exports) {
    exports.Set("TestReceiver", DefineClass(env, "TestReceiver", {
      InstanceMethod("unwrap", &TestReceiver::Unwrap),
    }));
  }
};


Napi::Object InitObjectWrapSaferUnwrap(Napi::Env env) {
  Napi::Object exports = Napi::Object::New(env);
  Test::Initialize(env, exports);
  return exports;
}
