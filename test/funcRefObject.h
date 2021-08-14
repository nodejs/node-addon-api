#ifndef FUNCREFOBJ_H
#define FUNCREFOBJ_H

#include <napi.h>

class FuncRefObject : public Napi::ObjectWrap<FuncRefObject> {
 public:
  FuncRefObject(const Napi::CallbackInfo& info);
  Napi::Value GetValue(const Napi::CallbackInfo& info);

 private:
  int _value;
};

#endif