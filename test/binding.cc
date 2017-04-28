#include "napi.h"

using namespace Napi;

Object InitArrayBuffer(Env env);
Object InitBuffer(Env env);
Object InitError(Env env);
Object InitExternal(Env env);
Object InitFunction(Env env);
Object InitName(Env env);
Object InitObject(Env env);

void Init(Env env, Object exports, Object module) {
  exports.Set("arraybuffer", InitArrayBuffer(env));
  exports.Set("buffer", InitBuffer(env));
  exports.Set("error", InitError(env));
  exports.Set("external", InitExternal(env));
  exports.Set("function", InitFunction(env));
  exports.Set("name", InitName(env));
  exports.Set("object", InitObject(env));
}

NODE_API_MODULE(addon, Init)
