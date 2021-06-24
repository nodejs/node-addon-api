#include <stdio.h>
#include "napi.h"

using namespace Napi;

static void cleanup(void* arg) {
  printf("static cleanup(%d)\n", *(int*)(arg));
}
static void cleanupInt(int* arg) {
  printf("static cleanup(%d)\n", *(arg));
}

static void cleanupVoid() {
  printf("static cleanup()\n");
}

static int secret1 = 42;
static int secret2 = 43;

Value AddHooks(const CallbackInfo& info) {
  auto env = info.Env();

  bool shouldRemove = info[0].As<Boolean>().Value();

  // hook: void (*)(void *arg), hint: int
  auto hook1 = AddCleanupHook(env, cleanup, &secret1);

  // hook: void (*)(int *arg), hint: int
  auto hook2 = AddCleanupHook(env, cleanupInt, &secret2);

  // hook: void (*)(int *arg), hint: void
  auto hook3 = AddCleanupHook(env, cleanupVoid);

  // hook: lambda []void (int *arg)->void, hint: int
  auto hook4 = AddCleanupHook(
      env, [&](int* arg) { printf("lambda cleanup(%d)\n", *arg); }, &secret1);

  // hook: lambda []void (void *)->void, hint: void
  auto hook5 = AddCleanupHook(
      env,
      [&](void*) { printf("lambda cleanup(void)\n"); },
      static_cast<void*>(nullptr));

  // hook: lambda []void ()->void, hint: void (default)
  auto hook6 = AddCleanupHook(env, [&]() { printf("lambda cleanup()\n"); });

  if (shouldRemove) {
    hook1.Remove(env);
    hook2.Remove(env);
    hook3.Remove(env);
    hook4.Remove(env);
    hook5.Remove(env);
    hook6.Remove(env);
  }

  return env.Undefined();
}

Object InitEnvCleanup(Env env) {
  Object exports = Object::New(env);

  exports["addHooks"] = Function::New(env, AddHooks);

  return exports;
}
