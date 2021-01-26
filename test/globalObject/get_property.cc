#include "napi.h"

using namespace Napi;


 
Value GetPropertyWithNapiValueAsKey(const CallbackInfo& info) {
    Object globalObject = info.Env().Global();
    Name key = info[0].As<Name>();
    return globalObject.Get(static_cast<napi_value>(key));
}

Value GetPropertyWithInt32AsKey(const CallbackInfo& info) {
    Object globalObject = info.Env().Global();
 
    Number key = info[0].As<Napi::Number>();
    return globalObject.Get(key.Uint32Value());
}


Value GetPropertyWithCStyleStringAsKey(const CallbackInfo& info) {
    Object globalObject = info.Env().Global();
    String cStrkey = info[0].As<String>();
    return globalObject.Get(cStrkey.Utf8Value().c_str());
}

Value GetPropertyWithCppStyleStringAsKey(const CallbackInfo& info){
    Object globalObject = info.Env().Global();
    String cppStrKey = info[0].As<String>();
    return globalObject.Get(cppStrKey.Utf8Value());
}

void CreateMockTestObject(const CallbackInfo& info){
    Object globalObject = info.Env().Global();

    napi_value napi_key;
    napi_create_int32(info.Env(), 2, &napi_key);

    globalObject.Set(napi_key,"napi_attribute");
    const char * CStringKey = "c_str_key";
    globalObject[CStringKey] = "c_string_attribute";
    globalObject[std::string("cpp_string_key")] = "cpp_string_attribute";
    globalObject[std::string("circular")] = globalObject;
    globalObject[(uint32_t)15] = 15;
    
}

Object InitGlobalObject(Env env) {
    Object exports = Object::New(env);
    exports["getPropertyWithInt32"] = Function::New(env, GetPropertyWithInt32AsKey);
    exports["getPropertyWithNapiValue"] = Function::New(env, GetPropertyWithNapiValueAsKey);
    exports["getPropertyWithCppString"] = Function::New(env, GetPropertyWithCppStyleStringAsKey);
    exports["getPropertyWithCString"] = Function::New(env, GetPropertyWithCStyleStringAsKey);
    exports["createMockTestObject"] = Function::New(env,CreateMockTestObject);
    return exports;
}
