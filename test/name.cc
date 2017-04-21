#include "napi.h"

using namespace Napi;

const char* testValueUtf8 = "123456789";
const char16_t* testValueUtf16 = u"123456789";

Value CreateString(const CallbackInfo& info) {
  String encoding = info[0].As<String>();
  Number length = info[1].As<Number>();

  if (encoding.Utf8Value() == "utf8") {
    if (length.IsUndefined()) {
      return String::New(info.Env(), testValueUtf8);
    } else {
      return String::New(info.Env(), testValueUtf8, length.Uint32Value());
    }
  } else if (encoding.Utf8Value() == "utf16") {
    if (length.IsUndefined()) {
      return String::New(info.Env(), testValueUtf16);
    } else {
      return String::New(info.Env(), testValueUtf16, length.Uint32Value());
    }
  } else {
    throw Error::New(info.Env(), "Invalid encoding.");
  }
}

Value CheckString(const CallbackInfo& info) {
  String value = info[0].As<String>();
  String encoding = info[1].As<String>();
  Number length = info[2].As<Number>();

  if (encoding.Utf8Value() == "utf8") {
    std::string testValue = testValueUtf8;
    if (!length.IsUndefined()) {
      testValue = testValue.substr(0, length.Uint32Value());
    }

    std::string stringValue = value;
    return Boolean::New(info.Env(), stringValue == testValue);
  } else if (encoding.Utf8Value() == "utf16") {
    std::u16string testValue = testValueUtf16;
    if (!length.IsUndefined()) {
      testValue = testValue.substr(0, length.Uint32Value());
    }

    std::u16string stringValue = value;
    return Boolean::New(info.Env(), stringValue == testValue);
  } else {
    throw Error::New(info.Env(), "Invalid encoding.");
  }
}

Value CreateSymbol(const CallbackInfo& info) {
  String description = info[0].As<String>();

  if (!description.IsUndefined()) {
    return Symbol::New(info.Env(), description);
  } else {
    return Symbol::New(info.Env());
  }
}

Value CheckSymbol(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].Type() == napi_symbol);
}

Object InitName(Env env) {
  Object exports = Object::New(env);

  exports["createString"] = Function::New(env, CreateString);
  exports["checkString"] = Function::New(env, CheckString);
  exports["createSymbol"] = Function::New(env, CreateSymbol);
  exports["checkSymbol"] = Function::New(env, CheckSymbol);

  return exports;
}
