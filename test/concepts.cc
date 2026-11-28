#include "napi.h"

using namespace Napi;

#if defined(NAPI_HAS_CPP20_CONCEPTS)

// Test CallbackCallable concept
static_assert(concepts::CallbackCallable<decltype([](const CallbackInfo& info) {
  return info.Env().Undefined();
})>);
static_assert(concepts::CallbackCallable<
              decltype([](const CallbackInfo& /*info*/) -> void {})>);
static_assert(concepts::CallbackCallable<decltype([](const CallbackInfo& info) {
  return String::New(info.Env(), "ok");
})>);

// Rejections:
// Non-copyable CallbackInfo passed by value
static_assert(!concepts::CallbackCallable<decltype([](CallbackInfo /*info*/) {
  return Value();
})>);
static_assert(!concepts::CallbackCallable<decltype([](int /*x*/) {})>);
static_assert(!concepts::CallbackCallable<decltype([]() {})>);
static_assert(!concepts::CallbackCallable<
              decltype([](const CallbackInfo& /*info*/, int /*extra*/) {})>);
static_assert(
    !concepts::CallbackCallable<
        decltype([](const CallbackInfo& /*info*/) -> int { return 42; })>);
static_assert(!concepts::CallbackCallable<int>);
static_assert(!concepts::CallbackCallable<void*>);

// Test GetterCallable concept
static_assert(concepts::GetterCallable<decltype([](const CallbackInfo& info) {
  return info.Env().Undefined();
})>);
static_assert(concepts::GetterCallable<decltype([](const CallbackInfo& info) {
  return Number::New(info.Env(), 42);
})>);
static_assert(!concepts::GetterCallable<
              decltype([](const CallbackInfo& /*info*/) -> void {})>);
static_assert(!concepts::GetterCallable<decltype([](int /*x*/) {})>);
static_assert(!concepts::GetterCallable<int>);

// Test SetterCallable concept
static_assert(
    concepts::SetterCallable<decltype([](const CallbackInfo& /*info*/) {})>);
static_assert(concepts::SetterCallable<
              decltype([](const CallbackInfo& /*info*/) -> void {})>);
static_assert(!concepts::SetterCallable<decltype([](const CallbackInfo& info) {
  return info.Env().Undefined();
})>);
static_assert(!concepts::SetterCallable<decltype([](int /*x*/) {})>);
static_assert(!concepts::SetterCallable<int>);

// Test Finalizer concept
static_assert(concepts::Finalizer<decltype([](Env /*env*/) {})>);
static_assert(!concepts::Finalizer<decltype([]() {})>);
static_assert(!concepts::Finalizer<decltype([](int /*x*/) {})>);
static_assert(
    !concepts::Finalizer<decltype([](Env /*env*/, int* /*data*/) {})>);
static_assert(!concepts::Finalizer<int>);

// Test FinalizerWithData concept
static_assert(
    concepts::FinalizerWithData<decltype([](Env /*env*/, int* /*data*/) {}),
                                int>);
static_assert(
    concepts::FinalizerWithData<decltype([](Env /*env*/, void* /*data*/) {}),
                                void>);
static_assert(
    !concepts::FinalizerWithData<decltype([](Env /*env*/, char* /*data*/) {}),
                                 int>);
static_assert(!concepts::FinalizerWithData<decltype([](Env /*env*/) {}), int>);
static_assert(!concepts::FinalizerWithData<decltype([](int /*x*/) {}), int>);

// Test FinalizerWithHint concept
static_assert(concepts::FinalizerWithHint<
              decltype([](Env /*env*/, int* /*data*/, char* /*hint*/) {}),
              int,
              char>);
static_assert(
    !concepts::FinalizerWithHint<decltype([](Env /*env*/, int* /*data*/) {}),
                                 int,
                                 char>);
static_assert(!concepts::FinalizerWithHint<
              decltype([](Env /*env*/, int* /*data*/, int* /*hint*/) {}),
              int,
              char>);

#endif  // defined(NAPI_HAS_CPP20_CONCEPTS)

Object InitConcepts(Env env) {
  Object exports = Object::New(env);

#if defined(NAPI_HAS_CPP20_CONCEPTS)
  exports["hasConcepts"] = Boolean::New(env, true);
#else
  exports["hasConcepts"] = Boolean::New(env, false);
#endif

  exports["testFunction"] = Function::New(env, [](const CallbackInfo& info) {
    return String::New(info.Env(), "concept callback ok");
  });

  Object obj = Object::New(env);
  obj.DefineProperty(PropertyDescriptor::Accessor(
      env,
      obj,
      "testProp",
      [](const CallbackInfo& info) { return Number::New(info.Env(), 42); },
      [](const CallbackInfo& /*info*/) {}));
  exports["testAccessor"] = obj;

  Object fnObj = Object::New(env);
  fnObj.DefineProperty(PropertyDescriptor::Function(
      env, fnObj, "testMethod", [](const CallbackInfo& info) {
        return Boolean::New(info.Env(), true);
      }));
  exports["testFunctionDescriptor"] = fnObj;

  static int finalizerData = 100;
  Object finalizerObj = Object::New(env);
  finalizerObj.AddFinalizer([](Env /*env*/, int* data) { *data = 200; },
                            &finalizerData);
  exports["testFinalizer"] = finalizerObj;

  static char abData[16] = {0};
  ArrayBuffer ab = ArrayBuffer::New(
      env, abData, sizeof(abData), [](Env /*env*/, void* /*data*/) {});
  exports["testArrayBuffer"] = ab;

  static uint8_t bufData[8] = {0};
  Buffer<uint8_t> buf = Buffer<uint8_t>::New(
      env, bufData, sizeof(bufData), [](Env /*env*/, uint8_t* /*data*/) {});
  exports["testBuffer"] = buf;

  return exports;
}
