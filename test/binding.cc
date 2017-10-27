#include "napi.h"

using namespace Napi;

Object InitArrayBuffer(Env env);
Object InitAsyncWorker(Env env);
Object InitBuffer(Env env);
Object InitError(Env env);
Object InitExternal(Env env);
Object InitFunction(Env env);
Object InitName(Env env);
Object InitObject(Env env);
Object InitPromise(Env env);
Object InitTypedArray(Env env);
Object InitObjectWrap(Env env);

Object Init(Env env, Object exports) {
  exports.Set("arraybuffer", InitArrayBuffer(env));
  exports.Set("asyncworker", InitAsyncWorker(env));
  exports.Set("buffer", InitBuffer(env));
  exports.Set("error", InitError(env));
  exports.Set("external", InitExternal(env));
  exports.Set("function", InitFunction(env));
  exports.Set("name", InitName(env));
  exports.Set("object", InitObject(env));
  exports.Set("promise", InitPromise(env));
  exports.Set("typedarray", InitTypedArray(env));
  exports.Set("objectwrap", InitObjectWrap(env));
  return exports;
}

NODE_API_MODULE(addon, Init)
