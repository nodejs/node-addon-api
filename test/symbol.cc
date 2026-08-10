#include <napi.h>

#include <string_view>
#include <utility>

#include "test_helper.h"
using namespace Napi;

namespace {

struct StringLike {
  operator std::string() const { return "unexpected-string-key"; }
  operator std::string_view() const { return value; }

  std::string value;
};

struct RvalueStringLike {
  operator std::string() && { return "unexpected-rvalue-string-key"; }
  operator std::string_view() && { return value; }

  std::string value;
};

struct StringOnlyLike {
  operator std::string() const { return value; }

  std::string value;
};

struct BothBases : std::string, std::string_view {};

struct ViewAndNapiString : std::string_view, Napi::String {};

struct StringReferenceLike {
  operator std::string&() const { return stringValue; }
  operator std::string&&() const { return std::move(stringValue); }
  operator std::string_view() const { return viewValue; }

  mutable std::string stringValue;
  std::string_view viewValue;
};

struct ImplicitAndExplicitStringViewLike {
  operator std::string() const { return "unexpected-string-key"; }

  // Copy-initialization must ignore the explicit conversion below.
  // Direct-initialization would prefer it for a non-const lvalue.
  operator std::string_view() const& { return value; }

  explicit operator std::string_view() & {
    return "unexpected-explicit-string-view-key";
  }

  std::string_view value;
};

}  // namespace

Symbol CreateNewSymbolWithNoArgs(const Napi::CallbackInfo&) {
  return Napi::Symbol();
}

Symbol CreateNewSymbolWithCppStrDesc(const Napi::CallbackInfo& info) {
  String cppStrKey = info[0].As<String>();
  return Napi::Symbol::New(info.Env(), cppStrKey.Utf8Value());
}

Symbol CreateNewSymbolWithCStrDesc(const Napi::CallbackInfo& info) {
  String cStrKey = info[0].As<String>();
  return Napi::Symbol::New(info.Env(), cStrKey.Utf8Value().c_str());
}

Symbol CreateNewSymbolWithNapiString(const Napi::CallbackInfo& info) {
  String strKey = info[0].As<String>();
  return Napi::Symbol::New(info.Env(), strKey);
}

Symbol GetWellknownSymbol(const Napi::CallbackInfo& info) {
  String registrySymbol = info[0].As<String>();
  return MaybeUnwrap(
      Napi::Symbol::WellKnown(info.Env(), registrySymbol.Utf8Value().c_str()));
}

Symbol FetchSymbolFromGlobalRegistry(const Napi::CallbackInfo& info) {
  String registrySymbol = info[0].As<String>();
  return MaybeUnwrap(Napi::Symbol::For(info.Env(), registrySymbol));
}

Symbol FetchSymbolFromGlobalRegistryWithCppKey(const Napi::CallbackInfo& info) {
  String cppStringKey = info[0].As<String>();
  return MaybeUnwrap(Napi::Symbol::For(info.Env(), cppStringKey.Utf8Value()));
}

Symbol FetchSymbolFromGlobalRegistryWithStringViewKey(
    const Napi::CallbackInfo& info) {
  String cppStringKey = info[0].As<String>();
  std::string key = cppStringKey.Utf8Value();
  return MaybeUnwrap(Napi::Symbol::For(info.Env(), std::string_view(key)));
}

Symbol FetchSymbolFromGlobalRegistryWithStringLikeKey(
    const Napi::CallbackInfo& info) {
  StringLike key{info[0].As<String>().Utf8Value()};
  return MaybeUnwrap(Napi::Symbol::For(info.Env(), key));
}

Symbol FetchSymbolFromGlobalRegistryWithRvalueStringLikeKey(
    const Napi::CallbackInfo& info) {
  return MaybeUnwrap(Napi::Symbol::For(
      info.Env(), RvalueStringLike{info[0].As<String>().Utf8Value()}));
}

Symbol FetchSymbolFromGlobalRegistryWithStringOnlyLikeKey(
    const Napi::CallbackInfo& info) {
  StringOnlyLike key{info[0].As<String>().Utf8Value()};
  return MaybeUnwrap(Napi::Symbol::For(info.Env(), key));
}

Symbol FetchSymbolFromGlobalRegistryWithBothBasesKey(
    const Napi::CallbackInfo& info) {
  std::string value = info[0].As<String>().Utf8Value();
  BothBases key;
  static_cast<std::string&>(key) = "unexpected-string-key";
  static_cast<std::string_view&>(key) = value;
  return MaybeUnwrap(Symbol::For(info.Env(), key));
}

Symbol FetchSymbolFromGlobalRegistryWithViewAndNapiStringKey(
    const Napi::CallbackInfo& info) {
  Env env = info.Env();
  std::string value = info[0].As<String>().Utf8Value();
  ViewAndNapiString key;
  static_cast<std::string_view&>(key) = value;
  static_cast<Napi::String&>(key) =
      Napi::String::New(env, "unexpected-napi-string-key");
  return MaybeUnwrap(Symbol::For(env, key));
}

Symbol FetchSymbolFromGlobalRegistryWithStringReferenceKey(
    const Napi::CallbackInfo& info) {
  std::string value = info[0].As<String>().Utf8Value();
  StringReferenceLike key{"unexpected-string-reference-key", value};
  return MaybeUnwrap(Symbol::For(info.Env(), key));
}

Symbol FetchSymbolFromGlobalRegistryWithImplicitViewKey(
    const Napi::CallbackInfo& info) {
  std::string value = info[0].As<String>().Utf8Value();
  ImplicitAndExplicitStringViewLike key{value};
  return MaybeUnwrap(Symbol::For(info.Env(), key));
}

Symbol FetchSymbolFromGlobalRegistryWithCKey(const Napi::CallbackInfo& info) {
  String cppStringKey = info[0].As<String>();
  return MaybeUnwrap(
      Napi::Symbol::For(info.Env(), cppStringKey.Utf8Value().c_str()));
}

Symbol TestUndefinedSymbolsCanBeCreated(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return MaybeUnwrap(Napi::Symbol::For(env, env.Undefined()));
}

Symbol TestNullSymbolsCanBeCreated(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return MaybeUnwrap(Napi::Symbol::For(env, env.Null()));
}

Object InitSymbol(Env env) {
  Object exports = Object::New(env);

  exports["createNewSymbolWithNoArgs"] =
      Function::New(env, CreateNewSymbolWithNoArgs);
  exports["createNewSymbolWithCppStr"] =
      Function::New(env, CreateNewSymbolWithCppStrDesc);
  exports["createNewSymbolWithCStr"] =
      Function::New(env, CreateNewSymbolWithCStrDesc);
  exports["createNewSymbolWithNapi"] =
      Function::New(env, CreateNewSymbolWithNapiString);
  exports["getWellKnownSymbol"] = Function::New(env, GetWellknownSymbol);
  exports["getSymbolFromGlobalRegistry"] =
      Function::New(env, FetchSymbolFromGlobalRegistry);
  exports["getSymbolFromGlobalRegistryWithCKey"] =
      Function::New(env, FetchSymbolFromGlobalRegistryWithCKey);
  exports["getSymbolFromGlobalRegistryWithCppKey"] =
      Function::New(env, FetchSymbolFromGlobalRegistryWithCppKey);
  exports["getSymbolFromGlobalRegistryWithStringViewKey"] =
      Function::New(env, FetchSymbolFromGlobalRegistryWithStringViewKey);
  exports["getSymbolFromGlobalRegistryWithStringLikeKey"] =
      Function::New(env, FetchSymbolFromGlobalRegistryWithStringLikeKey);
  exports["getSymbolFromGlobalRegistryWithRvalueStringLikeKey"] =
      Function::New(env, FetchSymbolFromGlobalRegistryWithRvalueStringLikeKey);
  exports["getSymbolFromGlobalRegistryWithStringOnlyLikeKey"] =
      Function::New(env, FetchSymbolFromGlobalRegistryWithStringOnlyLikeKey);
  exports["getSymbolFromGlobalRegistryWithBothBasesKey"] =
      Function::New(env, FetchSymbolFromGlobalRegistryWithBothBasesKey);
  exports["getSymbolFromGlobalRegistryWithViewAndNapiStringKey"] =
      Function::New(env, FetchSymbolFromGlobalRegistryWithViewAndNapiStringKey);
  exports["getSymbolFromGlobalRegistryWithStringReferenceKey"] =
      Function::New(env, FetchSymbolFromGlobalRegistryWithStringReferenceKey);
  exports["getSymbolFromGlobalRegistryWithImplicitViewKey"] =
      Function::New(env, FetchSymbolFromGlobalRegistryWithImplicitViewKey);
  exports["testUndefinedSymbolCanBeCreated"] =
      Function::New(env, TestUndefinedSymbolsCanBeCreated);
  exports["testNullSymbolCanBeCreated"] =
      Function::New(env, TestNullSymbolsCanBeCreated);
  return exports;
}
