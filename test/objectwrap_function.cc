#include <napi.h>
#include <unordered_map>
#include "test_helper.h"

class FunctionTest : public Napi::ObjectWrap<FunctionTest> {
 public:
  FunctionTest(const Napi::CallbackInfo& info)
      : Napi::ObjectWrap<FunctionTest>(info) {}

  static Napi::Value OnCalledAsFunction(const Napi::CallbackInfo& info) {
    // If called with a "true" argument, throw an exeption to test the handling.
    if (!info[0].IsUndefined() && MaybeUnwrap(info[0].ToBoolean())) {
      NAPI_THROW(Napi::Error::New(info.Env(), "an exception"), Napi::Value());
    }
    // Otherwise, act as a factory.
    std::vector<napi_value> args;
    for (size_t i = 0; i < info.Length(); i++) args.push_back(info[i]);
    return MaybeUnwrap(GetConstructor(info.Env()).New(args));
  }

  // Constructor-per-env map in a static member because env.SetInstanceData()
  // would interfere with Napi::Addon<T>
  static std::unordered_map<napi_env, Napi::FunctionReference> constructors;

  static void Initialize(Napi::Env env, Napi::Object exports) {
    const char* name = "FunctionTest";
    Napi::Function func = DefineClass(env, name, {});
    constructors[env] = Napi::Persistent(func);
    env.AddCleanupHook([env] { constructors.erase(env); });
    exports.Set(name, func);
  }

  static Napi::Function GetConstructor(Napi::Env env) {
    return constructors[env].Value();
  }
};

std::unordered_map<napi_env, Napi::FunctionReference>
    FunctionTest::constructors({{}});

Napi::Object InitObjectWrapFunction(Napi::Env env) {
  Napi::Object exports = Napi::Object::New(env);
  FunctionTest::Initialize(env, exports);
  return exports;
}
