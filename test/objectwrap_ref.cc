#include <napi.h>

class ObjectWrapRef : public Napi::ObjectWrap<ObjectWrapRef> {
 public:
  ObjectWrapRef(const Napi::CallbackInfo& info)
      : Napi::ObjectWrap<ObjectWrapRef>(info) {}

  Napi::Value DoRef(const Napi::CallbackInfo& info) {
    auto count = this->Ref();
    return Napi::Number::New(info.Env(), count);
  }

  Napi::Value DoUnref(const Napi::CallbackInfo& info) {
    auto count = this->Unref();
    return Napi::Number::New(info.Env(), count);
  }

  static void Initialize(Napi::Env env, Napi::Object exports) {
    const char* name = "ObjectWrapRef";
    exports.Set(
        name,
        DefineClass(
            env,
            name,
            {InstanceMethod("ref", &ObjectWrapRef::DoRef, napi_default),
             InstanceMethod("unref", &ObjectWrapRef::DoUnref, napi_default)}));
  }
};

Napi::Object InitObjectWrapRef(Napi::Env env) {
  Napi::Object exports = Napi::Object::New(env);
  ObjectWrapRef::Initialize(env, exports);
  return exports;
}
