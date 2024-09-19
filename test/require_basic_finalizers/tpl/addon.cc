#include <napi.h>

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(
      "external",
      Napi::External<int>::New(
          env, new int(1), [](Napi::Env /*env*/, int* data) { delete data; }));

  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
