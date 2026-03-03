#include "napi.h"

using namespace Napi;

static Value CreateDataView(const CallbackInfo& info) {
  ArrayBuffer arrayBuffer = info[0].As<ArrayBuffer>();
  return DataView::New(info.Env(), arrayBuffer);
}

static Value CreateDataViewWithByteOffset(const CallbackInfo& info) {
  ArrayBuffer arrayBuffer = info[0].As<ArrayBuffer>();
  size_t byteOffset = info[1].As<Number>().Uint32Value();
  return DataView::New(info.Env(), arrayBuffer, byteOffset);
}

static Value CreateDataViewWithByteOffsetAndByteLength(
    const CallbackInfo& info) {
  ArrayBuffer arrayBuffer = info[0].As<ArrayBuffer>();
  size_t byteOffset = info[1].As<Number>().Uint32Value();
  size_t byteLength = info[2].As<Number>().Uint32Value();
  return DataView::New(info.Env(), arrayBuffer, byteOffset, byteLength);
}

#ifdef NODE_API_EXPERIMENTAL_HAS_SHAREDARRAYBUFFER
static Value CreateDataViewOnSharedArrayBuffer(const CallbackInfo& info) {
  SharedArrayBuffer arrayBuffer = info[0].As<SharedArrayBuffer>();
  return DataView::New(info.Env(), arrayBuffer);
}

static Value CreateDataViewOnSharedArrayBufferWithByteOffset(
    const CallbackInfo& info) {
  SharedArrayBuffer arrayBuffer = info[0].As<SharedArrayBuffer>();
  size_t byteOffset = info[1].As<Number>().Uint32Value();
  return DataView::New(info.Env(), arrayBuffer, byteOffset);
}

static Value CreateDataViewOnSharedArrayBufferWithByteOffsetAndByteLength(
    const CallbackInfo& info) {
  SharedArrayBuffer arrayBuffer = info[0].As<SharedArrayBuffer>();
  size_t byteOffset = info[1].As<Number>().Uint32Value();
  size_t byteLength = info[2].As<Number>().Uint32Value();
  return DataView::New(info.Env(), arrayBuffer, byteOffset, byteLength);
}
#endif

static Value GetBuffer(const CallbackInfo& info) {
  return info[0].As<DataView>().Buffer();
}

static Value GetArrayBuffer(const CallbackInfo& info) {
  return info[0].As<DataView>().ArrayBuffer();
}

static Value GetByteOffset(const CallbackInfo& info) {
  return Number::New(info.Env(),
                     static_cast<double>(info[0].As<DataView>().ByteOffset()));
}

static Value GetByteLength(const CallbackInfo& info) {
  return Number::New(info.Env(),
                     static_cast<double>(info[0].As<DataView>().ByteLength()));
}

Object InitDataView(Env env) {
  Object exports = Object::New(env);

  exports["createDataView"] = Function::New(env, CreateDataView);
  exports["createDataViewWithByteOffset"] =
      Function::New(env, CreateDataViewWithByteOffset);
  exports["createDataViewWithByteOffsetAndByteLength"] =
      Function::New(env, CreateDataViewWithByteOffsetAndByteLength);

#ifdef NODE_API_EXPERIMENTAL_HAS_SHAREDARRAYBUFFER
  exports["createDataViewOnSharedArrayBuffer"] =
      Function::New(env, CreateDataViewOnSharedArrayBuffer);
  exports["createDataViewOnSharedArrayBufferWithByteOffset"] =
      Function::New(env, CreateDataViewOnSharedArrayBufferWithByteOffset);
  exports["createDataViewOnSharedArrayBufferWithByteOffsetAndByteLength"] =
      Function::New(
          env, CreateDataViewOnSharedArrayBufferWithByteOffsetAndByteLength);
#endif

  exports["getArrayBuffer"] = Function::New(env, GetArrayBuffer);
  exports["getBuffer"] = Function::New(env, GetBuffer);
  exports["getByteOffset"] = Function::New(env, GetByteOffset);
  exports["getByteLength"] = Function::New(env, GetByteLength);

  return exports;
}
