#include "napi.h"

using namespace Napi;

Object InitError(Env env);
Object InitFunction(Env env);

void Init(Env env, Object exports, Object module) {
  exports.Set("error", InitError(env));
  exports.Set("function", InitFunction(env));
}

NODE_API_MODULE(addon, Init)
