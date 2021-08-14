#include "funcRefObject.h"
#include "test_helper.h"

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
