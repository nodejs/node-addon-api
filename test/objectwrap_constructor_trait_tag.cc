#include <napi.h>
#include "test_helper.h"

struct EventTargetConstructorTraitTag {
  static napi_value GetThis(const Napi::CallbackInfo& info) {
    Napi::Function eventTargetCons =
        MaybeUnwrap(info.Env().Global().Get("EventTarget"))
            .As<Napi::Function>();
    Napi::Object eventTarget = MaybeUnwrap(eventTargetCons.New({}));
    return eventTarget;
  }
};

class TestOverride : public Napi::ObjectWrap<TestOverride> {
 public:
  TestOverride(const Napi::CallbackInfo& info)
      : Napi::ObjectWrap<TestOverride>(info, EventTargetConstructorTraitTag()) {
  }

  Napi::Value IsTestOverride(const Napi::CallbackInfo& info) {
    TestOverride* self = Unwrap(info.This().As<Napi::Object>());
    return Napi::Boolean::New(info.Env(), self != nullptr);
  }

  static void Initialize(Napi::Env env, Napi::Object exports) {
    Napi::Function cons = DefineClass(
        env,
        "TestOverride",
        {InstanceAccessor<&TestOverride::IsTestOverride>("isTestOverride")});

    exports.Set("TestOverride", cons);
  }
};

Napi::Object InitObjectWrapConstructorTraitTag(Napi::Env env) {
  Napi::Object exports = Napi::Object::New(env);
  TestOverride::Initialize(env, exports);
  return exports;
}
