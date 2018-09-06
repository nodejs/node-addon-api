#include "napi.h"

using namespace Napi;

Value CreateUtf8String(const CallbackInfo& info) {
	return String::New(info.Env(), info[0].As<String>().Utf8Value());  
}

Value CreateUtf16String(const CallbackInfo& info) {
	return String::New(info.Env(), info[0].As<String>().Utf16Value());  
}

Object InitBasicTypesString(Env env) {
	Object exports = Object::New(env);

	exports["createUtf8String"] = Function::New(env, CreateUtf8String);
	exports["createUtf16String"] = Function::New(env, CreateUtf16String);

  return exports;
}
