#include "napi.h"

using namespace Napi;

Value IsWholeNumber(const CallbackInfo& info) {
  String input = info[0].As<String>();
  if (input.IsNumber()) {
    double num = input.As<Number>().DoubleValue();;
    if (((int64_t)(num) == num) && num>=0) {
      return Boolean::New(info.Env(),true);
    }
  }
  return Boolean::New(info.Env(),false);
}

Value EvenOrOdd(const CallbackInfo& info) {
  int64_t input = info[0].As<Number>().DoubleValue();
  if(input%2 == 0) {
    return String::New(info.Env(), "Even");
  }
  return String::New(info.Env(), "Odd");
}

Object InitEvenOdd(Env env) {
  Object exports = Object::New(env);
  exports["IsWholeNumber"] = Function::New(env, IsWholeNumber);
  exports["EvenOrOdd"] = Function::New(env, EvenOrOdd);
  return exports;
}
