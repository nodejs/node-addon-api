#include "napi.h"

using namespace Napi;

Value jsonParse(const CallbackInfo& info) {
    Env env = info.Env();
    return JSON::Parse(env, info[0].As<String>());
}

Value jsonStringify(const CallbackInfo& info) {
    Env env = info.Env();
    Value replacer = Value();
    Value space = Value();
    if (info.Length() > 1) {
      replacer = info[1].As<Value>();
    }
    if (info.Length() > 2) {
      space = info[2].As<Value>();
    }
    return JSON::Stringify(env, info[0].As<Object>(), replacer, space);
}

Object InitJson(Env env) {
    Object exports = Object::New(env);
    exports["jsonParse"] = Function::New(env, jsonParse);
    exports["jsonStringify"] = Function::New(env, jsonStringify);
    return exports;
}
