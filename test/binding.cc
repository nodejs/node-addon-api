#define NAPI_EXPERIMENTAL
#include "napi.h"

using namespace Napi;

Object InitArrayBuffer(Env env);
Object InitAsyncContext(Env env);
Object InitAsyncWorker(Env env);
Object InitPersistentAsyncWorker(Env env);
Object InitBasicTypesArray(Env env);
Object InitBasicTypesBoolean(Env env);
Object InitBasicTypesNumber(Env env);
Object InitBasicTypesValue(Env env);
// currently experimental guard with version of NAPI_VERSION that it is
// released in once it is no longer experimental
#if (NAPI_VERSION > 2147483646)
Object InitBigInt(Env env);
#endif
Object InitBuffer(Env env);
#if (NAPI_VERSION > 2)
Object InitCallbackScope(Env env);
#endif
Object InitDataView(Env env);
Object InitDataViewReadWrite(Env env);
Object InitError(Env env);
Object InitExternal(Env env);
Object InitFunction(Env env);
Object InitHandleScope(Env env);
Object InitMemoryManagement(Env env);
Object InitName(Env env);
Object InitObject(Env env);
#ifndef NODE_ADDON_API_DISABLE_DEPRECATED
Object InitObjectDeprecated(Env env);
#endif // !NODE_ADDON_API_DISABLE_DEPRECATED
Object InitPromise(Env env);
Object InitTypedArray(Env env);
Object InitObjectWrap(Env env);
Object InitObjectReference(Env env);
Object InitVersionManagement(Env env);
Object InitThunkingManual(Env env);

Object Init(Env env, Object exports) {
  exports.Set("persistentasyncworker", InitPersistentAsyncWorker(env));
  return exports;
}

NODE_API_MODULE(addon, Init)
