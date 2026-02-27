#include "napi.h"

#if __cplusplus >= 201703L
#include <string_view>
#endif

using namespace Napi;

const char* testValueUtf8 = "123456789";
const char16_t* testValueUtf16 = NAPI_WIDE_TEXT("123456789");

Value EchoString(const CallbackInfo& info) {
  String value = info[0].As<String>();
  String encoding = info[1].As<String>();

  if (encoding.Utf8Value() == "utf8") {
    return String::New(info.Env(), value.Utf8Value().c_str());
  } else if (encoding.Utf8Value() == "utf16") {
    return String::New(info.Env(), value.Utf16Value().c_str());
  } else {
    Error::New(info.Env(), "Invalid encoding.").ThrowAsJavaScriptException();
    return Value();
  }
}

Value CreateString(const CallbackInfo& info) {
  String encoding = info[0].As<String>();
  Value length = info[1];

  if (encoding.Utf8Value() == "utf8") {
    if (length.IsUndefined()) {
      return String::New(info.Env(), testValueUtf8);
    } else {
      return String::New(
          info.Env(), testValueUtf8, length.As<Number>().Uint32Value());
    }
  } else if (encoding.Utf8Value() == "utf16") {
    if (length.IsUndefined()) {
      return String::New(info.Env(), testValueUtf16);
    } else {
      return String::New(
          info.Env(), testValueUtf16, length.As<Number>().Uint32Value());
    }
  } else {
    Error::New(info.Env(), "Invalid encoding.").ThrowAsJavaScriptException();
    return Value();
  }
}

#if __cplusplus >= 201703L
Value CreateStringFromStringView(const CallbackInfo& info) {
  return String::New(info.Env(), std::string_view("hello1"));
}
#endif

Value CheckString(const CallbackInfo& info) {
  String value = info[0].As<String>();
  String encoding = info[1].As<String>();
  Value length = info[2];

  if (encoding.Utf8Value() == "utf8") {
    std::string testValue = testValueUtf8;
    if (!length.IsUndefined()) {
      testValue = testValue.substr(0, length.As<Number>().Uint32Value());
    }

    std::string stringValue = value;
    return Boolean::New(info.Env(), stringValue == testValue);
  } else if (encoding.Utf8Value() == "utf16") {
    std::u16string testValue = testValueUtf16;
    if (!length.IsUndefined()) {
      testValue = testValue.substr(0, length.As<Number>().Uint32Value());
    }

    std::u16string stringValue = value;
    return Boolean::New(info.Env(), stringValue == testValue);
  } else {
    Error::New(info.Env(), "Invalid encoding.").ThrowAsJavaScriptException();
    return Value();
  }
}

Value CreateSymbol(const CallbackInfo& info) {
  Value description = info[0];

  if (!description.IsUndefined()) {
    return Symbol::New(info.Env(), description.As<String>());
  } else {
    return Symbol::New(info.Env());
  }
}

#if __cplusplus >= 201703L
Value CreateSymbolFromStringView(const CallbackInfo& info) {
  return Symbol::New(info.Env(), std::string_view("hello2"));
}
#endif

Value CheckSymbol(const CallbackInfo& info) {
  return Boolean::New(info.Env(), info[0].Type() == napi_symbol);
}

void NullStringShouldThrow(const CallbackInfo& info) {
  const char* nullStr = nullptr;
  String::New(info.Env(), nullStr);
}

void NullString16ShouldThrow(const CallbackInfo& info) {
  const char16_t* nullStr = nullptr;
  String::New(info.Env(), nullStr);
}

Object InitName(Env env) {
  Object exports = Object::New(env);

  exports["echoString"] = Function::New(env, EchoString);
  exports["createString"] = Function::New(env, CreateString);
  exports["createStringFromStringView"] = Function::New(env, CreateStringFromStringView);
  exports["nullStringShouldThrow"] = Function::New(env, NullStringShouldThrow);
  exports["nullString16ShouldThrow"] =
      Function::New(env, NullString16ShouldThrow);
  exports["checkString"] = Function::New(env, CheckString);
  exports["createSymbol"] = Function::New(env, CreateSymbol);
  exports["CreateSymbolFromStringView"] = Function::New(env, CreateSymbolFromStringView);
  exports["checkSymbol"] = Function::New(env, CheckSymbol);

  return exports;
}
