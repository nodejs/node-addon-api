#include "napi.h"

using namespace Napi;

namespace {

int testData = 1;

void VoidCallback(const CallbackInfo& info) {
  auto env = info.Env();
  Object obj = info[0].As<Object>();

  obj["foo"] = String::New(env, "bar");
}

Value ValueCallback(const CallbackInfo& info) {
  auto env = info.Env();
  Object obj = Object::New(env);

  obj["foo"] = String::New(env, "bar");

  return obj;
}

void VoidCallbackWithData(const CallbackInfo& info) {
  auto env = info.Env();
  Object obj = info[0].As<Object>();

  obj["foo"] = String::New(env, "bar");

  int* data = static_cast<int*>(info.Data());
  obj["data"] = Number::New(env, *data);
}

Value ValueCallbackWithData(const CallbackInfo& info) {
  auto env = info.Env();
  Object obj = Object::New(env);

  obj["foo"] = String::New(env, "bar");

  int* data = static_cast<int*>(info.Data());
  obj["data"] = Number::New(env, *data);

  return obj;
}

Value CallWithArgs(const CallbackInfo& info) {
   Function func = info[0].As<Function>();
   return func({ info[1], info[2], info[3] });
}

Value CallWithVector(const CallbackInfo& info) {
   Function func = info[0].As<Function>();
   std::vector<napi_value> args;
   args.reserve(3);
   args.push_back(info[1]);
   args.push_back(info[2]);
   args.push_back(info[3]);
   return func.Call(args);
}

Value CallWithReceiverAndArgs(const CallbackInfo& info) {
   Function func = info[0].As<Function>();
   Value receiver = info[1];
   return func.Call(receiver, { info[2], info[3], info[4] });
}

Value CallWithReceiverAndVector(const CallbackInfo& info) {
   Function func = info[0].As<Function>();
   Value receiver = info[1];
   std::vector<napi_value> args;
   args.reserve(3);
   args.push_back(info[2]);
   args.push_back(info[3]);
   args.push_back(info[4]);
   return func.Call(receiver, args);
}

Value CallConstructorWithArgs(const CallbackInfo& info) {
   Function func = info[0].As<Function>();
   return func.New({ info[1], info[2], info[3] });
}

Value CallConstructorWithVector(const CallbackInfo& info) {
   Function func = info[0].As<Function>();
   std::vector<napi_value> args;
   args.reserve(3);
   args.push_back(info[1]);
   args.push_back(info[2]);
   args.push_back(info[3]);
   return func.New(args);
}

} // end anonymous namespace

Object InitFunction(Env env) {
  Object exports = Object::New(env);
  exports["voidCallback"] = Function::New(env, VoidCallback, "voidCallback");
  exports["valueCallback"] = Function::New(env, ValueCallback, std::string("valueCallback"));
  exports["voidCallbackWithData"] =
    Function::New(env, VoidCallbackWithData, nullptr, &testData);
  exports["valueCallbackWithData"] =
    Function::New(env, ValueCallbackWithData, nullptr, &testData);
  exports["callWithArgs"] = Function::New(env, CallWithArgs);
  exports["callWithVector"] = Function::New(env, CallWithVector);
  exports["callWithReceiverAndArgs"] = Function::New(env, CallWithReceiverAndArgs);
  exports["callWithReceiverAndVector"] = Function::New(env, CallWithReceiverAndVector);
  exports["callConstructorWithArgs"] = Function::New(env, CallConstructorWithArgs);
  exports["callConstructorWithVector"] = Function::New(env, CallConstructorWithVector);
  return exports;
}
