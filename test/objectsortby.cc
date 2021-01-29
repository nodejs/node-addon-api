#include <bits/stdc++.h>
#include "napi.h"
namespace {

Napi::Array sorybyWrapped(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  int length = info.Length();
  if (length != 2) {
    Napi::TypeError::New(env, "Two argument expected")
        .ThrowAsJavaScriptException();
  }
  if (!info[0].IsArray()) {
    Napi::TypeError::New(env, "Frist argument is expected to be an array")
        .ThrowAsJavaScriptException();
  }

  if (!info[1].IsString()) {
    Napi::TypeError::New(env, "Wrong second argument, should be an integer")
        .ThrowAsJavaScriptException();
  }

  const Napi::Array inputArray = info[0].As<Napi::Array>();
  Napi::String attr = info[1].As<Napi::String>();

  int arrlength = inputArray.Length();
  int value;
  std::vector<std::pair<int, int>> vect;

  Napi::Object temp;
  Napi::Value val1;

  for (int i = 0; i < arrlength; i++) {
    temp = inputArray[i].As<Napi::Object>();
    val1 = temp.Get(attr);
    value = val1.As<Napi::Number>().Int32Value();
    vect.push_back(std::make_pair(value, i));
  }

  Napi::Array resultArray = Napi::Array::New(env, arrlength);
  sort(vect.begin(), vect.end());

  for (int i = 0; i < arrlength; i++) {
    int index = vect[i].second;
    resultArray[i] = inputArray[index];
  }

  return resultArray;
}

}  // namespace

Napi::Object InitObjectSortby(Napi::Env env) {
  Napi::Object exports = Napi::Object::New(env);

  exports["sortby"] = Napi::Function::New(env, sorybyWrapped);
  return exports;
}