#include "napi.h"

using namespace Napi;

Value Test1(const CallbackInfo& info) {
  auto env = info.Env();
  Object obj = Object::New(env);

  obj["foo"] = String::New(env, "bar");

  return obj;
}

void Init(Env env, Object exports, Object module) {
  exports.Set("test1", Function::New(env, Test1));
}

NODE_API_MODULE(addon, Init)
