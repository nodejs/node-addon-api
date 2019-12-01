#include "napi.h"

using namespace Napi;

namespace {

Value RunPlainString(const CallbackInfo& info) {
  Env env = info.Env();
  return env.RunScript("1 + 2 + 3");
}

Value RunStdString(const CallbackInfo& info) {
  Env env = info.Env();
  std::string str = "1 + 2 + 3";
  return env.RunScript(str);
}

Value RunJsString(const CallbackInfo& info) {
  Env env = info.Env();
  return env.RunScript(info[0].As<String>());
}

Value RunWithContext(const CallbackInfo& info) {
  Env env = info.Env();

  Array keys = info[1].As<Object>().GetPropertyNames();
  std::string code = "(";
  for (unsigned int i = 0; i < keys.Length(); i++) {
    if (i != 0) code += ",";
    code += keys.Get(i).As<String>().Utf8Value();
  }
  code += ") => " + info[0].As<String>().Utf8Value();

  Value ret = env.RunScript(code);
  Function fn = ret.As<Function>();
  std::vector<napi_value> args;
  for (unsigned int i = 0; i < keys.Length(); i++) {
    Value key = keys.Get(i);
    args.push_back(info[1].As<Object>().Get(key));
  }
  return fn.Call(args);
}

} // end anonymous namespace

Object InitRunScript(Env env) {
  Object exports = Object::New(env);

  exports["plainString"] = Function::New(env, RunPlainString);
  exports["stdString"] = Function::New(env, RunStdString);
  exports["jsString"] = Function::New(env, RunJsString);
  exports["withContext"] = Function::New(env, RunWithContext);

  return exports;
}
