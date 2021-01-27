#include "napi.h"

using namespace Napi;

// Wrappers for testing Object::Get() for global Objects
Value GetPropertyWithCppStyleStringAsKey(const CallbackInfo& info);
Value GetPropertyWithCStyleStringAsKey(const CallbackInfo& info);
Value GetPropertyWithInt32AsKey(const CallbackInfo& info);
Value GetPropertyWithNapiValueAsKey(const CallbackInfo& info);
void CreateMockTestObject(const CallbackInfo& info);

// Wrapper for testing Object::Set() for global Objects
Value SetPropertyWithCStyleStringAsKey(const CallbackInfo& info);
Value SetPropertyWithCppStyleStringAsKey(const CallbackInfo& info);
Value SetPropertyWithInt32AsKey(const CallbackInfo& info);
Value SetPropertyWithNapiValueAsKey(const CallbackInfo& info);

Value HasPropertyWithCStyleStringAsKey(const CallbackInfo& info);
Value HasPropertyWithCppStyleStringAsKey(const CallbackInfo& info);
Value HasPropertyWithInt32AsKey(const CallbackInfo& info);
Value HasPropertyWithNapiValueAsKey(const CallbackInfo& info);

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
  exports["setPropertyWithCStyleString"] =
      Function::New(env, SetPropertyWithCStyleStringAsKey);
  exports["setPropertyWithCppStyleString"] =
      Function::New(env, SetPropertyWithCppStyleStringAsKey);
  exports["setPropertyWithNapiValue"] =
      Function::New(env, SetPropertyWithNapiValueAsKey);
  exports["setPropertyWithInt32"] =
      Function::New(env, SetPropertyWithInt32AsKey);
  exports["hasPropertyWithCStyleString"] =
      Function::New(env,HasPropertyWithCStyleStringAsKey);
  exports["hasPropertyWithCppStyleString"] =
      Function::New(env,HasPropertyWithCppStyleStringAsKey);
  exports["hasPropertyWithInt32"] =
      Function::New(env, HasPropertyWithInt32AsKey);   
  exports["hasPropertyWithNapiValueAsKey"] = 
      Function::New(env, HasPropertyWithNapiValueAsKey);
  return exports;
}
