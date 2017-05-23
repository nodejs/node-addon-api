#include "napi.h"

using namespace Napi;

namespace {

Value CreateTypedArray(const CallbackInfo& info) {
  std::string arrayType = info[0].As<String>();
  size_t length = info[1].As<Number>().Uint32Value();
  ArrayBuffer buffer = info[2].As<ArrayBuffer>();
  size_t bufferOffset = info[3].IsUndefined() ? 0 : info[3].As<Number>().Uint32Value();

  if (arrayType == "int8") {
    return buffer.IsUndefined() ? Int8Array::New(info.Env(), length)
      : Int8Array::New(info.Env(), length, buffer, bufferOffset);
  } else if (arrayType == "uint8") {
    return buffer.IsUndefined() ? Uint8Array::New(info.Env(), length)
      : Uint8Array::New(info.Env(), length, buffer, bufferOffset);
  } else if (arrayType == "uint8_clamped") {
    return buffer.IsUndefined() ? Uint8Array::New(info.Env(), length, napi_uint8_clamped_array)
      : Uint8Array::New(info.Env(), length, buffer, bufferOffset, napi_uint8_clamped_array);
  } else if (arrayType == "int16") {
    return buffer.IsUndefined() ? Int16Array::New(info.Env(), length)
      : Int16Array::New(info.Env(), length, buffer, bufferOffset);
  } else if (arrayType == "uint16") {
    return buffer.IsUndefined() ? Uint16Array::New(info.Env(), length)
      : Uint16Array::New(info.Env(), length, buffer, bufferOffset);
  } else if (arrayType == "int32") {
    return buffer.IsUndefined() ? Int32Array::New(info.Env(), length)
      : Int32Array::New(info.Env(), length, buffer, bufferOffset);
  } else if (arrayType == "uint32") {
    return buffer.IsUndefined() ? Uint32Array::New(info.Env(), length)
      : Uint32Array::New(info.Env(), length, buffer, bufferOffset);
  } else if (arrayType == "float32") {
    return buffer.IsUndefined() ? Float32Array::New(info.Env(), length)
      : Float32Array::New(info.Env(), length, buffer, bufferOffset);
  } else if (arrayType == "float64") {
    return buffer.IsUndefined() ? Float64Array::New(info.Env(), length)
      : Float64Array::New(info.Env(), length, buffer, bufferOffset);
  } else {
    throw Error::New(info.Env(), "Invalid typed-array type.");
  }
}

Value GetTypedArrayType(const CallbackInfo& info) {
  TypedArray array = info[0].As<TypedArray>();
  switch (array.TypedArrayType()) {
    case napi_int8_array: return String::New(info.Env(), "int8");
    case napi_uint8_array: return String::New(info.Env(), "uint8");
    case napi_uint8_clamped_array: return String::New(info.Env(), "uint8_clamped");
    case napi_int16_array: return String::New(info.Env(), "int16");
    case napi_uint16_array: return String::New(info.Env(), "uint16");
    case napi_int32_array: return String::New(info.Env(), "int32");
    case napi_uint32_array: return String::New(info.Env(), "uint32");
    case napi_float32_array: return String::New(info.Env(), "float32");
    case napi_float64_array: return String::New(info.Env(), "float64");
    default: return String::New(info.Env(), "invalid");
  }
}

Value GetTypedArrayLength(const CallbackInfo& info) {
  TypedArray array = info[0].As<TypedArray>();
  return Number::New(info.Env(), static_cast<double>(array.ElementLength()));
}

Value GetTypedArrayBuffer(const CallbackInfo& info) {
  TypedArray array = info[0].As<TypedArray>();
  return array.ArrayBuffer();
}

Value GetTypedArrayElement(const CallbackInfo& info) {
  TypedArray array = info[0].As<TypedArray>();
  size_t index = info[1].As<Number>().Uint32Value();
  switch (array.TypedArrayType()) {
    case napi_int8_array:
      return Number::New(info.Env(), array.As<Int8Array>()[index]);
    case napi_uint8_array:
      return Number::New(info.Env(), array.As<Uint8Array>()[index]);
    case napi_uint8_clamped_array:
      return Number::New(info.Env(), array.As<Uint8Array>()[index]);
    case napi_int16_array:
      return Number::New(info.Env(), array.As<Int16Array>()[index]);
    case napi_uint16_array:
      return Number::New(info.Env(), array.As<Uint16Array>()[index]);
    case napi_int32_array:
      return Number::New(info.Env(), array.As<Int32Array>()[index]);
    case napi_uint32_array:
      return Number::New(info.Env(), array.As<Uint32Array>()[index]);
    case napi_float32_array:
      return Number::New(info.Env(), array.As<Float32Array>()[index]);
    case napi_float64_array:
      return Number::New(info.Env(), array.As<Float64Array>()[index]);
    default: throw Error::New(info.Env(), "Invalid typed-array type.");
  }
}

void SetTypedArrayElement(const CallbackInfo& info) {
  TypedArray array = info[0].As<TypedArray>();
  size_t index = info[1].As<Number>().Uint32Value();
  Number value = info[2].As<Number>();
  switch (array.TypedArrayType()) {
    case napi_int8_array:
      array.As<Int8Array>()[index] = static_cast<int8_t>(value.Int32Value());
      break;
    case napi_uint8_array:
      array.As<Uint8Array>()[index] = static_cast<uint8_t>(value.Uint32Value());
      break;
    case napi_uint8_clamped_array:
      array.As<Uint8Array>()[index] = static_cast<uint8_t>(value.Uint32Value());
      break;
    case napi_int16_array:
      array.As<Int16Array>()[index] = static_cast<int16_t>(value.Int32Value());
      break;
    case napi_uint16_array:
      array.As<Uint16Array>()[index] = static_cast<uint16_t>(value.Uint32Value());
      break;
    case napi_int32_array:
      array.As<Int32Array>()[index] = value.Int32Value();
      break;
    case napi_uint32_array:
      array.As<Uint32Array>()[index] = value.Uint32Value();
      break;
    case napi_float32_array:
      array.As<Float32Array>()[index] = value.FloatValue();
      break;
    case napi_float64_array:
      array.As<Float64Array>()[index] = value.DoubleValue();
      break;
    default:
      throw Error::New(info.Env(), "Invalid typed-array type.");
  }
}

} // end anonymous namespace

Object InitTypedArray(Env env) {
  Object exports = Object::New(env);

  exports["createTypedArray"] = Function::New(env, CreateTypedArray);
  exports["getTypedArrayType"] = Function::New(env, GetTypedArrayType);
  exports["getTypedArrayLength"] = Function::New(env, GetTypedArrayLength);
  exports["getTypedArrayBuffer"] = Function::New(env, GetTypedArrayBuffer);
  exports["getTypedArrayElement"] = Function::New(env, GetTypedArrayElement);
  exports["setTypedArrayElement"] = Function::New(env, SetTypedArrayElement);

  return exports;
}
