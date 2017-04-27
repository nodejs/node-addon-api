#include "napi.h"

using namespace Napi;

Object InitError(Env env);
Object InitFunction(Env env);
Object InitName(Env env);

void Init(Env env, Object exports, Object module) {
  exports.Set("error", InitError(env));
  exports.Set("function", InitFunction(env));
  exports.Set("name", InitName(env));
}

NODE_API_MODULE(addon, Init)
