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

// Callables that can only be called on a named object, not a temporary, are
// accepted, since the library keeps the callable and calls it from storage.
struct LvalueOnlyCallback {
  Value operator()(const CallbackInfo& info) & {
    return info.Env().Undefined();
  }
};
struct LvalueOnlySetter {
  void operator()(const CallbackInfo& /*info*/) & {}
};
struct LvalueOnlyFinalizer {
  void operator()(Env /*env*/) & {}
  void operator()(Env /*env*/, int* /*data*/) & {}
  void operator()(Env /*env*/, int* /*data*/, char* /*hint*/) & {}
};
static_assert(concepts::CallbackCallable<LvalueOnlyCallback>);
static_assert(concepts::GetterCallable<LvalueOnlyCallback>);
static_assert(concepts::SetterCallable<LvalueOnlySetter>);
static_assert(concepts::Finalizer<LvalueOnlyFinalizer>);
static_assert(concepts::FinalizerWithData<LvalueOnlyFinalizer, int>);
static_assert(concepts::FinalizerWithHint<LvalueOnlyFinalizer, int, char>);

// Callables that can only be called on a temporary, so they may use up their
// own state, are rejected, since the library calls the copy it keeps and
// never lets it be used up.
struct RvalueOnlyCallback {
  Value operator()(const CallbackInfo& info) && {
    return info.Env().Undefined();
  }
};
struct RvalueOnlySetter {
  void operator()(const CallbackInfo& /*info*/) && {}
};
struct RvalueOnlyFinalizer {
  void operator()(Env /*env*/) && {}
  void operator()(Env /*env*/, int* /*data*/) && {}
  void operator()(Env /*env*/, int* /*data*/, char* /*hint*/) && {}
};
static_assert(!concepts::CallbackCallable<RvalueOnlyCallback>);
static_assert(!concepts::GetterCallable<RvalueOnlyCallback>);
static_assert(!concepts::SetterCallable<RvalueOnlySetter>);
static_assert(!concepts::Finalizer<RvalueOnlyFinalizer>);
static_assert(!concepts::FinalizerWithData<RvalueOnlyFinalizer, int>);
static_assert(!concepts::FinalizerWithHint<RvalueOnlyFinalizer, int, char>);

// Check that the APIs themselves accept or reject these callables, not just the
// concepts.
template <typename F>
constexpr bool CanCreateFunction =
    requires(Env env, F f) { Function::New(env, f); };
template <typename Getter, typename Setter>
constexpr bool CanCreateAccessor =
    requires(Env env, Object obj, Getter getter, Setter setter) {
      PropertyDescriptor::Accessor(env, obj, "x", getter, setter);
    };
template <typename F>
constexpr bool CanAddFinalizer =
    requires(Object obj, F f, int* data) { obj.AddFinalizer(f, data); };

static_assert(CanCreateFunction<LvalueOnlyCallback>);
static_assert(!CanCreateFunction<RvalueOnlyCallback>);
static_assert(CanCreateAccessor<LvalueOnlyCallback, LvalueOnlySetter>);
static_assert(!CanCreateAccessor<RvalueOnlyCallback, LvalueOnlySetter>);
static_assert(!CanCreateAccessor<LvalueOnlyCallback, RvalueOnlySetter>);
static_assert(CanAddFinalizer<LvalueOnlyFinalizer>);
static_assert(!CanAddFinalizer<RvalueOnlyFinalizer>);

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

  auto namedLambda = [](const CallbackInfo& info) {
    return String::New(info.Env(), "named lambda ok");
  };
  exports["testNamedLambda"] = Function::New(env, namedLambda);

  // The counter lives in the library's stored copy of the lambda, so it keeps
  // counting across calls from JavaScript.
  exports["testMutableLambda"] =
      Function::New(env, [count = 0](const CallbackInfo& info) mutable {
        return Number::New(info.Env(), ++count);
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

#if defined(BUILDING_CPP20_MODULE)
static Object InitConceptsModule(Env env, Object exports) {
  exports.Set("concepts", InitConcepts(env));
  return exports;
}

NODE_API_MODULE(addon, InitConceptsModule)
#endif
