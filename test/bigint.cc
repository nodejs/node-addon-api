// Currently experimental guard with NODE_MAJOR_VERISION in which it was
// released. Once it is no longer experimental guard with the NAPI_VERSION
// in which it is released instead.
#if (NODE_MAJOR_VERSION >= 10)

#define NAPI_EXPERIMENTAL
#include "napi.h"

using namespace Napi;

namespace {

Value IsLossless(const CallbackInfo& info) {
  Env env = info.Env();

  BigInt big = info[0].As<BigInt>();
  bool is_signed = info[1].ToBoolean().Value();

  bool lossless;
  if (is_signed) {
    big.Int64Value(&lossless);
  } else {
    big.Uint64Value(&lossless);
  }

  return Boolean::New(env, lossless);
}

Value TestInt64(const CallbackInfo& info) {
  bool lossless;
  int64_t input = info[0].As<BigInt>().Int64Value(&lossless);

  return BigInt::New(info.Env(), input);
}

Value TestUint64(const CallbackInfo& info) {
  bool lossless;
  uint64_t input = info[0].As<BigInt>().Uint64Value(&lossless);

  return BigInt::New(info.Env(), input);
}

Value TestWords(const CallbackInfo& info) {
  BigInt big = info[0].As<BigInt>();

  size_t expected_word_count = big.WordCount();

  int sign_bit;
  size_t word_count = 10;
  uint64_t words[10];

  big.ToWords(&sign_bit, &word_count, words);

  if (word_count != expected_word_count) {
    Error::New(info.Env(), "word count did not match").ThrowAsJavaScriptException();
    return BigInt();
  }

  return BigInt::New(info.Env(), sign_bit, word_count, words);
}

Value TestTooBigBigInt(const CallbackInfo& info) {
  int sign_bit = 0;
  size_t word_count = SIZE_MAX;
  uint64_t words[10];

  return BigInt::New(info.Env(), sign_bit, word_count, words);
}

}  // anonymous namespace

Object InitBigInt(Env env) {
  Object exports = Object::New(env);
  exports["IsLossless"] = Function::New(env, IsLossless);
  exports["TestInt64"] = Function::New(env, TestInt64);
  exports["TestUint64"] = Function::New(env, TestUint64);
  exports["TestWords"] = Function::New(env, TestWords);
  exports["TestTooBigBigInt"] = Function::New(env, TestTooBigBigInt);

  return exports;
}

#endif
