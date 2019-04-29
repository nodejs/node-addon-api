#include <napi.h>
#include <assert.h>

#ifdef NAPI_CPP_EXCEPTIONS
namespace {

static int dtor_called = 0;

class DtorCounter {
 public:
  ~DtorCounter() {
    assert(dtor_called == 0);
    dtor_called++;
  }
};

Napi::Value GetDtorCalled(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), dtor_called);
}

class Test : public Napi::ObjectWrap<Test> {
public:
  Test(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Test>(info) {
    throw Napi::Error::New(Env(), "Some error");
  }

  static void Initialize(Napi::Env env, Napi::Object exports) {
    exports.Set("Test", DefineClass(env, "Test", {}));
    exports.Set("getDtorCalled", Napi::Function::New(env, GetDtorCalled));
  }

private:
  DtorCounter dtor_ounter_;
};

}  // anonymous namespace
#endif  // NAPI_CPP_EXCEPTIONS

Napi::Object InitObjectWrapRemoveWrap(Napi::Env env) {
  Napi::Object exports = Napi::Object::New(env);
#ifdef NAPI_CPP_EXCEPTIONS
  Test::Initialize(env, exports);
#endif
  return exports;
}
