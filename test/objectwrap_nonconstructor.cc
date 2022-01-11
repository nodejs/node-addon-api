#include <napi.h>
#include "test_helper.h"
#include <unordered_map>

class NonConstructorTest :
  public Napi::ObjectWrap<NonConstructorTest> {
public:
  NonConstructorTest(const Napi::CallbackInfo& info) :
    Napi::ObjectWrap<NonConstructorTest>(info) {}

  static Napi::Value NonConstructor(const Napi::CallbackInfo& info) {
    // If called with a "true" argument, throw an exeption to test the handling.
    if(!info[0].IsUndefined() && MaybeUnwrap(info[0].ToBoolean())) {
      NAPI_THROW(Napi::Error::New(info.Env(), "an exception"), Napi::Value());
    }
    // Otherwise, act as a factory.
    std::vector<napi_value> args;
    for(size_t i = 0; i < info.Length(); i++) args.push_back(info[i]);
    return MaybeUnwrap(GetConstructor(info.Env()).New(args));
  }

  static std::unordered_map<napi_env, Napi::FunctionReference*> constructors;

  static void Initialize(Napi::Env env, Napi::Object exports) {
    const char* name = "NonConstructorTest";
    Napi::Function func = DefineClass(env, name, {});
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    constructors[(napi_env)env] = constructor;
    exports.Set(name, func);
  }

  static Napi::Function GetConstructor(Napi::Env env) {
    Napi::EscapableHandleScope scope(env);
    Napi::Function func = constructors[(napi_env)env]->Value();
    return scope.Escape(napi_value(func)).As<Napi::Function>();
  }
};

std::unordered_map<napi_env, Napi::FunctionReference*> NonConstructorTest::constructors = {};

Napi::Object InitObjectWrapNonConstructor(Napi::Env env) {
  Napi::Object exports = Napi::Object::New(env);
  NonConstructorTest::Initialize(env, exports);
  return exports;
}
