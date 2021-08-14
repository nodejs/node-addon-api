#include "funcRefObject.h"
#include "test_helper.h"

Napi::Object FuncRefObject::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func =
      DefineClass(env,
                  "FuncRefObject",
                  {InstanceMethod("CreateNewObj", &FuncRefObject::GetValue)});
  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor);
  exports.Set("FuncRefObject", func);
  return exports;
}

FuncRefObject::FuncRefObject(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<FuncRefObject>(info) {
  Napi::Env env = info.Env();
  int argLen = info.Length();
  if (argLen <= 0 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "First param should be a number")
        .ThrowAsJavaScriptException();
    return;
  }
  Napi::Number value = info[0].As<Napi::Number>();
  this->_value = value.Int32Value();
}

Napi::Value FuncRefObject::GetValue(const Napi::CallbackInfo& info) {
  int value = this->_value;
  return Napi::Number::New(info.Env(), value);
}
