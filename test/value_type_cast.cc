#include "common/test_helper.h"
#include "napi.h"

using namespace Napi;

#define TYPE_CAST_TYPES(V)                                                     \
  V(Boolean)                                                                   \
  V(Number)                                                                    \
  V(BigInt)                                                                    \
  V(Date)                                                                      \
  V(String)                                                                    \
  V(Symbol)                                                                    \
  V(Object)                                                                    \
  V(Array)                                                                     \
  V(ArrayBuffer)                                                               \
  V(TypedArray)                                                                \
  V(DataView)                                                                  \
  V(Function)                                                                  \
  V(Promise)

// The following types are tested individually.
// External
// TypedArrayOf
// Buffer

namespace {
#define V(Type)                                                                \
  void TypeCast##Type(const CallbackInfo& info) { USE(info[0].As<Type>()); }
TYPE_CAST_TYPES(V)

#ifdef NODE_API_EXPERIMENTAL_HAS_SHAREDARRAYBUFFER
V(SharedArrayBuffer)
#endif

#undef V

void TypeCastBuffer(const CallbackInfo& info) {
  USE(info[0].As<Buffer<uint8_t>>());
}

void TypeCastExternal(const CallbackInfo& info) {
  USE(info[0].As<External<void>>());
}

void TypeCastTypeArrayOfUint8(const CallbackInfo& info) {
  USE(info[0].As<TypedArrayOf<uint8_t>>());
}
}  // namespace

Object InitValueTypeCast(Env env, Object exports) {
  exports["external"] = External<void>::New(env, nullptr);

#define V(Type) exports["typeCast" #Type] = Function::New(env, TypeCast##Type);
  TYPE_CAST_TYPES(V)

#ifdef NODE_API_EXPERIMENTAL_HAS_SHAREDARRAYBUFFER
  V(SharedArrayBuffer)
#endif

#undef V

  exports["typeCastBuffer"] = Function::New(env, TypeCastBuffer);
  exports["typeCastExternal"] = Function::New(env, TypeCastExternal);
  exports["typeCastTypeArrayOfUint8"] =
      Function::New(env, TypeCastTypeArrayOfUint8);

  return exports;
}

NODE_API_MODULE(addon, InitValueTypeCast)
