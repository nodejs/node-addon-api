#include "napi.h"
#include "test_helper.h"

using namespace Napi;

namespace {

int testData = 1;

// Helpers for testing non-Javascript values.
Value CreateExternal(const CallbackInfo& info) {
  return External<int>::New(info.Env(), &testData);
}

}  // end anonymous namespace

static Value StrictlyEquals(const CallbackInfo& info) {
  bool strictlyEquals = info[0].StrictEquals(info[1]);
  return Boolean::New(info.Env(), strictlyEquals);
}

// tests the  '==' overload
static Value StrictEqualsOverload(const CallbackInfo& info) {
  bool strictlyEquals = info[0] == info[1];
  return Boolean::New(info.Env(), strictlyEquals);
}

// tests the '!=' overload
static Value StrictlyNotEqualsOverload(const CallbackInfo& info) {
  bool strictlyEquals = info[0] != info[1];
  return Boolean::New(info.Env(), strictlyEquals);
}

static Value ValueReturnsCorrectEnv(const CallbackInfo& info) {
  Value testValue = CreateExternal(info);
  return Boolean::New(info.Env(), testValue.Env() == info.Env());
}

static Value TestEnvComparisons(const CallbackInfo& info) {
  Env env = info.Env();
  Env env2 = info.Env();
  bool eq = (env == env2);
  bool ne = (env != env2);
  bool eqNull = (env == nullptr);
  bool neNull = (env != nullptr);
  bool nullEq = (nullptr == env);
  bool nullNe = (nullptr != env);

  bool result = eq && !ne && !eqNull && neNull && !nullEq && nullNe;
#if defined(NAPI_HAS_CPP20_COMPARISONS)
  result = result && ((env <=> env2) == 0);
#endif
  return Boolean::New(info.Env(), result);
}

static Value HasCpp20Comparisons(const CallbackInfo& info) {
#if defined(NAPI_HAS_CPP20_COMPARISONS)
  return Boolean::New(info.Env(), true);
#else
  return Boolean::New(info.Env(), false);
#endif
}

static Value TestMaybeComparisons(const CallbackInfo& info) {
  Maybe<int> m_none1 = Nothing<int>();
  Maybe<int> m_none2 = Nothing<int>();
  Maybe<int> m_one = Just<int>(1);
  Maybe<int> m_two = Just<int>(2);

  bool ok = (m_none1 == m_none2) && (m_none1 != m_one) && (m_one == m_one) &&
            (m_one != m_two);
#if defined(NAPI_HAS_CPP20_COMPARISONS)
  ok = ok && (m_none1 < m_one) && (m_one > m_none1) && (m_one < m_two) &&
       (m_two > m_one) && (m_one <= m_two) && (m_two >= m_one) &&
       (m_one <= m_one) && (m_one >= m_one) && ((m_none1 <=> m_none2) == 0) &&
       ((m_none1 <=> m_one) < 0) && ((m_one <=> m_none1) > 0) &&
       ((m_one <=> m_two) < 0) && ((m_two <=> m_one) > 0);

  Maybe<double> md_none = Nothing<double>();
  Maybe<double> md_one = Just<double>(1.5);
  Maybe<double> md_two = Just<double>(2.5);
  ok = ok && (md_none < md_one) && (md_one < md_two) &&
       ((md_one <=> md_two) < 0);
#endif
  return Boolean::New(info.Env(), ok);
}

static Value EmptyValueReturnNullPtrOnCast(const CallbackInfo& info) {
  Value emptyValue;
  bool isNullPtr = static_cast<napi_value>(emptyValue) == nullptr;
  return Boolean::New(info.Env(), isNullPtr);
}

static Value NonEmptyValueReturnValOnCast(const CallbackInfo& info) {
  Value boolValue = Value::From(info.Env(), true);
  return Boolean::New(info.Env(), static_cast<napi_value>(boolValue));
}

static Value CreateNonEmptyValue(const CallbackInfo& info) {
  return Napi::Value(info.Env(), String::New(info.Env(), "non_empty_val"));
}

static Value IsEmpty(const CallbackInfo& info) {
  Value value;
  return Boolean::New(info.Env(), value.IsEmpty());
}

static Value IsUndefined(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsUndefined());
}

static Value IsNull(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsNull());
}

static Value IsBoolean(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsBoolean());
}

static Value IsNumber(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsNumber());
}

static Value IsString(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsString());
}

static Value IsSymbol(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsSymbol());
}

static Value IsArray(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsArray());
}

static Value IsArrayBuffer(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsArrayBuffer());
}

static Value IsTypedArray(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsTypedArray());
}

static Value IsObject(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsObject());
}

static Value IsFunction(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsFunction());
}

static Value IsPromise(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsPromise());
}

static Value IsDataView(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsDataView());
}

static Value IsExternal(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].IsExternal());
}

static Value ToBoolean(const CallbackInfo& info) {
  return MaybeUnwrap(info[0].ToBoolean());
}

static Value ToNumber(const CallbackInfo& info) {
  return MaybeUnwrap(info[0].ToNumber());
}

static Value ToString(const CallbackInfo& info) {
  return MaybeUnwrap(info[0].ToString());
}

static Value ToObject(const CallbackInfo& info) {
  return MaybeUnwrap(info[0].ToObject());
}

static Value AccessProp(const CallbackInfo& info) {
  Object obj = MaybeUnwrap(info[0].ToObject());
  return obj[info[1]].AsValue();
}

Object InitBasicTypesValue(Env env) {
  Object exports = Object::New(env);

  exports["isEmpty"] = Function::New(env, IsEmpty);
  exports["isUndefined"] = Function::New(env, IsUndefined);
  exports["isNull"] = Function::New(env, IsNull);
  exports["isBoolean"] = Function::New(env, IsBoolean);
  exports["isNumber"] = Function::New(env, IsNumber);
  exports["isString"] = Function::New(env, IsString);
  exports["isSymbol"] = Function::New(env, IsSymbol);
  exports["isArray"] = Function::New(env, IsArray);
  exports["isArrayBuffer"] = Function::New(env, IsArrayBuffer);
  exports["isTypedArray"] = Function::New(env, IsTypedArray);
  exports["isObject"] = Function::New(env, IsObject);
  exports["isFunction"] = Function::New(env, IsFunction);
  exports["isPromise"] = Function::New(env, IsPromise);
  exports["isDataView"] = Function::New(env, IsDataView);
  exports["isExternal"] = Function::New(env, IsExternal);
  exports["toBoolean"] = Function::New(env, ToBoolean);
  exports["toNumber"] = Function::New(env, ToNumber);
  exports["toString"] = Function::New(env, ToString);
  exports["toObject"] = Function::New(env, ToObject);
  exports["accessProp"] = Function::New(env, AccessProp);

  exports["strictlyEquals"] = Function::New(env, StrictlyEquals);
  exports["strictlyEqualsOverload"] = Function::New(env, StrictEqualsOverload);
  exports["strictlyNotEqualsOverload"] =
      Function::New(env, StrictlyNotEqualsOverload);

  exports["assertValueReturnsCorrectEnv"] =
      Function::New(env, ValueReturnsCorrectEnv);
  exports["assertEnvComparisons"] = Function::New(env, TestEnvComparisons);
  exports["assertMaybeComparisons"] = Function::New(env, TestMaybeComparisons);
  exports["hasCpp20Comparisons"] = Function::New(env, HasCpp20Comparisons);

  exports["assertEmptyValReturnNullPtrOnCast"] =
      Function::New(env, EmptyValueReturnNullPtrOnCast);
  exports["assertNonEmptyReturnValOnCast"] =
      Function::New(env, NonEmptyValueReturnValOnCast);

  exports["createNonEmptyValue"] = Function::New(env, CreateNonEmptyValue);
  exports["createExternal"] = Function::New(env, CreateExternal);

  return exports;
}

#if defined(BUILDING_CPP20_MODULE)
static Object InitValueModule(Env env, Object exports) {
  exports.Set("basic_types_value", InitBasicTypesValue(env));
  return exports;
}

NODE_API_MODULE(addon, InitValueModule)
#endif
