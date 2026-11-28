#include "napi.h"

using namespace Napi;

Object InitBasicTypesValue(Env env);
Object InitConcepts(Env env);

static Object InitCpp20(Env env, Object exports) {
  exports.Set("basic_types_value", InitBasicTypesValue(env));
  exports.Set("concepts", InitConcepts(env));
  return exports;
}

NODE_API_MODULE(addon, InitCpp20)

