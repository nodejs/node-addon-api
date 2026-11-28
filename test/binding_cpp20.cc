#include "napi.h"

using namespace Napi;

Object InitBuffer(Env env);
Object InitBufferNoExternal(Env env);
Object InitConcepts(Env env);

static Object InitCpp20(Env env, Object exports) {
  exports.Set("buffer", InitBuffer(env));
  exports.Set("bufferNoExternal", InitBufferNoExternal(env));
  exports.Set("concepts", InitConcepts(env));
  return exports;
}

NODE_API_MODULE(addon, InitCpp20)
