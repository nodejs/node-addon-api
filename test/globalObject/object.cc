#include "napi.h"

using namespace Napi;

// Wrappers for testing Object::Get() for global Objects
Value GetPropertyWithCppStyleStringAsKey(const CallbackInfo& info);
Value GetPropertyWithCStyleStringAsKey(const CallbackInfo& info);
Value GetPropertyWithInt32AsKey(const CallbackInfo& info);
Value GetPropertyWithNapiValueAsKey(const CallbackInfo& info);
void CreateMockTestObject(const CallbackInfo& info);

Object InitGlobalObject(Env env) {
  Object exports = Object::New(env);
  exports["getPropertyWithInt32"] =
      Function::New(env, GetPropertyWithInt32AsKey);
  exports["getPropertyWithNapiValue"] =
      Function::New(env, GetPropertyWithNapiValueAsKey);
  exports["getPropertyWithCppString"] =
      Function::New(env, GetPropertyWithCppStyleStringAsKey);
  exports["getPropertyWithCString"] =
      Function::New(env, GetPropertyWithCStyleStringAsKey);
  exports["createMockTestObject"] = Function::New(env, CreateMockTestObject);
  return exports;
}