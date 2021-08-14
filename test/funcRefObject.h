#ifndef FUNCREFOBJ_H
#define FUNCREFOBJ_H

#include <napi.h>

class FuncRefObject : public Napi::ObjectWrap<FuncRefObject> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  FuncRefObject(const Napi::CallbackInfo& info);
  Napi::Value GetValue(const Napi::CallbackInfo& info);

 private:
  int _value;
};

#endif