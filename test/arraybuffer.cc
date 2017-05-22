#include "napi.h"

using namespace Napi;

namespace {

const size_t testLength = 4;
uint8_t testData[testLength];
int finalizeCount = 0;

void InitData(uint8_t* data, size_t length) {
  for (size_t i = 0; i < length; i++) {
    data[i] = static_cast<uint8_t>(i);
  }
}

bool VerifyData(uint8_t* data, size_t length) {
  for (size_t i = 0; i < length; i++) {
    if (data[i] != static_cast<uint8_t>(i)) {
      return false;
    }
  }
  return true;
}

Value CreateBuffer(const CallbackInfo& info) {
  ArrayBuffer buffer = ArrayBuffer::New(info.Env(), testLength);

  if (buffer.ByteLength() != testLength) {
    throw Error::New(info.Env(), "Incorrect buffer length.");
  }

  InitData(static_cast<uint8_t*>(buffer.Data()), testLength);
  return buffer;
}

Value CreateExternalBuffer(const CallbackInfo& info) {
  ArrayBuffer buffer = ArrayBuffer::New(info.Env(), testData, testLength);

  if (buffer.ByteLength() != testLength) {
    throw Error::New(info.Env(), "Incorrect buffer length.");
  }

  if (buffer.Data() != testData) {
    throw Error::New(info.Env(), "Incorrect buffer data.");
  }

  InitData(testData, testLength);
  return buffer;
}

Value CreateExternalBufferWithFinalize(const CallbackInfo& info) {
  uint8_t* data = new uint8_t[testLength];

  ArrayBuffer buffer = ArrayBuffer::New(
    info.Env(),
    data,
    testLength,
    [](Env env, void* finalizeData) {
      delete[] static_cast<uint8_t*>(finalizeData);
      finalizeCount++;
    });

  if (buffer.ByteLength() != testLength) {
    throw Error::New(info.Env(), "Incorrect buffer length.");
  }

  if (buffer.Data() != data) {
    throw Error::New(info.Env(), "Incorrect buffer data.");
  }

  InitData(data, testLength);
  return buffer;
}

Value CreateExternalBufferWithFinalizeHint(const CallbackInfo& info) {
  uint8_t* data = new uint8_t[testLength];

  char* hint = nullptr;
  ArrayBuffer buffer = ArrayBuffer::New(
    info.Env(),
    data,
    testLength,
    [](Env env, void* finalizeData, char* finalizeHint) {
      delete[] static_cast<uint8_t*>(finalizeData);
      finalizeCount++;
    },
    hint);

  if (buffer.ByteLength() != testLength) {
    throw Error::New(info.Env(), "Incorrect buffer length.");
  }

  if (buffer.Data() != data) {
    throw Error::New(info.Env(), "Incorrect buffer data.");
  }

  InitData(data, testLength);
  return buffer;
}

void CheckBuffer(const CallbackInfo& info) {
  if (!info[0].IsArrayBuffer()) {
    throw Error::New(info.Env(), "A buffer was expected.");
  }

  ArrayBuffer buffer = info[0].As<ArrayBuffer>();

  if (buffer.ByteLength() != testLength) {
    throw Error::New(info.Env(), "Incorrect buffer length.");
  }

  if (!VerifyData(static_cast<uint8_t*>(buffer.Data()), testLength)) {
    throw Error::New(info.Env(), "Incorrect buffer data.");
  }
}

Value GetFinalizeCount(const CallbackInfo& info) {
   return Number::New(info.Env(), finalizeCount);
}

} // end anonymous namespace

Object InitArrayBuffer(Env env) {
  Object exports = Object::New(env);

  exports["createBuffer"] = Function::New(env, CreateBuffer);
  exports["createExternalBuffer"] = Function::New(env, CreateExternalBuffer);
  exports["createExternalBufferWithFinalize"] =
    Function::New(env, CreateExternalBufferWithFinalize);
  exports["createExternalBufferWithFinalizeHint"] =
    Function::New(env, CreateExternalBufferWithFinalizeHint);
  exports["checkBuffer"] = Function::New(env, CheckBuffer);
  exports["getFinalizeCount"] = Function::New(env, GetFinalizeCount);

  return exports;
}
