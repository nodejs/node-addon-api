#include "napi.h"

using namespace Napi;

Object InitArrayBuffer(Env env);
Object InitAsyncWorker(Env env);
Object InitBasicTypesNumber(Env env);
Object InitBasicTypesValue(Env env);
Object InitBigInt(Env env);
Object InitBuffer(Env env);
Object InitDataView(Env env);
Object InitDataViewReadWrite(Env env);
Object InitError(Env env);
Object InitExternal(Env env);
Object InitFunction(Env env);
Object InitHandleScope(Env env);
Object InitMemoryManagement(Env env);
Object InitName(Env env);
Object InitObject(Env env);
Object InitPromise(Env env);
Object InitTypedArray(Env env);
Object InitObjectWrap(Env env);
Object InitObjectReference(Env env);

Object Init(Env env, Object exports) {
  exports.Set("arraybuffer", InitArrayBuffer(env));
  exports.Set("asyncworker", InitAsyncWorker(env));
  exports.Set("basic_types_number", InitBasicTypesNumber(env));
  exports.Set("basic_types_value", InitBasicTypesValue(env));
  exports.Set("bigint", InitBigInt(env));
  exports.Set("buffer", InitBuffer(env));
  exports.Set("dataview", InitDataView(env));
  exports.Set("dataview_read_write", InitDataView(env));
  exports.Set("dataview_read_write", InitDataViewReadWrite(env));
  exports.Set("error", InitError(env));
  exports.Set("external", InitExternal(env));
  exports.Set("function", InitFunction(env));
  exports.Set("name", InitName(env));
  exports.Set("handlescope", InitHandleScope(env));
  exports.Set("memory_management", InitMemoryManagement(env));
  exports.Set("object", InitObject(env));
  exports.Set("promise", InitPromise(env));
  exports.Set("typedarray", InitTypedArray(env));
  exports.Set("objectwrap", InitObjectWrap(env));
  exports.Set("objectreference", InitObjectReference(env));
  return exports;
}

NODE_API_MODULE(addon, Init)
