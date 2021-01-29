#include "napi.h"
namespace {
void addby(int* arr, int length, int a) {
  for (int i = 0; i < length; i++) arr[i] += a;
}

Napi::Array addbyWrapped(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  int length = info.Length();
  if (length != 2) {
    Napi::TypeError::New(env, "Two argument expected")
        .ThrowAsJavaScriptException();
  }
  if (!info[0].IsArray()) {
    Napi::TypeError::New(env, "Frist argument is expected to be an array")
        .ThrowAsJavaScriptException();
  }

  if (!info[1].IsNumber()) {
    Napi::TypeError::New(env, "Wrong second argument, should be an integer")
        .ThrowAsJavaScriptException();
  }

  const Napi::Array inputArray = info[0].As<Napi::Array>();
  Napi::Number a = info[1].As<Napi::Number>();
  int arrlength = inputArray.Length();
  int array[arrlength];
  Napi::Number temp;

  for (int i = 0; i < arrlength; i++) {
    temp = inputArray[i].As<Napi::Number>();
    array[i] = temp.Int32Value();
  }

  addby(&array[0], arrlength, a.Int32Value());

  Napi::Array resultArray = Napi::Array::New(env, arrlength);

  for (int i = 0; i < arrlength; i++) {
    resultArray[i] = Napi::Number::New(env, double(array[i]));
  }

  return resultArray;
}

Napi::Array ShowValue(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  int length = info.Length();
  if (length != 1) {
    Napi::TypeError::New(env, "Only one argument expected")
        .ThrowAsJavaScriptException();
  }

  Napi::Array a = info[0].As<Napi::Array>();

  return a;
}

}  // namespace

Napi::Object InitArrayOperations(Napi::Env env) {
  Napi::Object exports = Napi::Object::New(env);

  exports["addby"] = Napi::Function::New(env, addbyWrapped);
  exports["ShowValue"] = Napi::Function::New(env, ShowValue);
  return exports;
}
