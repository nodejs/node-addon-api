#include "assert.h"
#include "napi.h"
#if defined(NODE_ADDON_API_ENABLE_MAYBE)

using namespace Napi;

namespace {

void VoidCallback(const CallbackInfo& info) {
  Napi::Function fn = info[0].As<Function>();
  Maybe<Value> ret = fn.Call({});

  assert(ret.IsNothing() == true);
  assert(ret.IsJust() == false);

  Napi::Value placeHolder = Napi::Number::New(info.Env(), 12345);
  Napi::Value unwrappedValue = ret.UnwrapOr(placeHolder);

  assert(unwrappedValue.As<Number>().Uint32Value() == 12345);

  assert(ret.UnwrapTo(&placeHolder) == false);
  assert(placeHolder.As<Number>().Uint32Value() == 12345);

  ret.Check();
}

void TestMaybeOperatorOverload(const CallbackInfo& info) {
  Napi::Function fn_a = info[0].As<Function>();
  Napi::Function fn_b = info[1].As<Function>();

  assert(fn_a.Call({}) == fn_a.Call({}));
  assert(fn_a.Call({}) != fn_b.Call({}));
#if defined(NAPI_HAS_CPP20_COMPARISONS)
  Maybe<int> m_none1 = Nothing<int>();
  Maybe<int> m_none2 = Nothing<int>();
  Maybe<int> m_one = Just<int>(1);
  Maybe<int> m_two = Just<int>(2);

  assert(m_none1 == m_none2);
  assert(m_none1 != m_one);
  assert(m_none1 < m_one);
  assert(m_one > m_none1);
  assert(m_one < m_two);
  assert(m_two > m_one);
  assert(m_one <= m_two);
  assert(m_two >= m_one);
  assert(m_one <= m_one);
  assert(m_one >= m_one);
  assert((m_none1 <=> m_none2) == 0);
  assert((m_none1 <=> m_one) < 0);
  assert((m_one <=> m_none1) > 0);
  assert((m_one <=> m_two) < 0);
  assert((m_two <=> m_one) > 0);
#endif
}

void NormalJsCallback(const CallbackInfo& info) {
  Napi::Function fn = info[0].As<Function>();
  uint32_t magic_number = info[1].As<Number>().Uint32Value();

  Maybe<Value> ret = fn.Call({});

  assert(ret.IsNothing() == false);
  assert(ret.IsJust() == true);

  Napi::Value unwrappedValue = ret.Unwrap();
  assert(unwrappedValue.IsNumber() == true);

  assert(unwrappedValue.As<Number>().Uint32Value() == magic_number);

  unwrappedValue =
      ret.UnwrapOr(Napi::Number::New(info.Env(), magic_number - 1));
  assert(unwrappedValue.As<Number>().Uint32Value() == magic_number);

  Napi::Value placeHolder = Napi::Number::New(info.Env(), magic_number - 1);
  assert(ret.UnwrapTo(&placeHolder) == true);
  assert(placeHolder.As<Number>().Uint32Value() == magic_number);
}

}  // end anonymous namespace

Object InitMaybeCheck(Env env) {
  Object exports = Object::New(env);
  exports.Set("voidCallback", Function::New(env, VoidCallback));
  exports.Set("normalJsCallback", Function::New(env, NormalJsCallback));
  exports.Set("testMaybeOverloadOp",
              Function::New(env, TestMaybeOperatorOverload));
  return exports;
}

#endif
