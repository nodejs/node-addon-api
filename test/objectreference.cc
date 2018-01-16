/* ObjectReference can be used to create references to Values that
are not Objects by creating a blank Object and setting Values to
it. Subclasses of Objects can only be set using an ObjectReference
by first casting it as an Object. */

#include "napi.h"

using namespace Napi;

ObjectReference weak;
ObjectReference persistent;
ObjectReference reference;

ObjectReference casted_weak;
ObjectReference casted_persistent;
ObjectReference casted_reference;

void SetObjects(const CallbackInfo& info) {
  Env env = info.Env();
  HandleScope scope(env);

  weak = Weak(Object::New(env));
  persistent = Persistent(Object::New(env));
  reference = Reference<Object>::New(Object::New(env), 2);

  if (info[0].IsString()) {
    if (info[2].As<String>() == String::New(env, "javascript")) {
      weak.Set(info[0].As<String>(), info[1]);
      persistent.Set(info[0].As<String>(), info[1]);
      reference.Set(info[0].As<String>(), info[1]);
    } else {
      weak.Set(info[0].As<String>().Utf8Value(), info[1]);
      persistent.Set(info[0].As<String>().Utf8Value(), info[1]);
      reference.Set(info[0].As<String>().Utf8Value(), info[1]);
    }
  } else if (info[0].IsNumber()) {
    weak.Set(info[0].As<Number>(), info[1]);
    persistent.Set(info[0].As<Number>(), info[1]);
    reference.Set(info[0].As<Number>(), info[1]);
  }
}

void SetCastedObjects(const CallbackInfo& info) {
  Env env = info.Env();
  HandleScope scope(env);

  Array ex = Array::New(env);
  ex.Set((uint32_t)0, String::New(env, "hello"));
  ex.Set(1, String::New(env, "world"));
  ex.Set(2, String::New(env, "!"));

  casted_weak = Weak(ex.As<Object>());
  casted_persistent = Persistent(ex.As<Object>());
  casted_reference = Reference<Object>::New(ex.As<Object>(), 2);
}


Value GetFromValue(const CallbackInfo& info) {
  Env env = info.Env();

  if (info[0].As<String>() == String::New(env, "weak")) {
    if (weak.IsEmpty()) {
      return String::New(env, "No Referenced Value");
    } else {
      return weak.Value();
    }
  } else if (info[0].As<String>() == String::New(env, "persistent")) {
    return persistent.Value();
  } else {
    return reference.Value();
  }
}

Value GetFromGetter(const CallbackInfo& info) {
  Env env = info.Env();

  if (info[0].As<String>() == String::New(env, "weak")) {
    if (weak.IsEmpty()) {
      return String::New(env, "No Referenced Value");
    } else {
      if (info[1].IsString()) {
        return weak.Get(info[1].As<String>().Utf8Value());
      } else if (info[1].IsNumber()) {
        return weak.Get(info[1].As<Number>().Uint32Value());
      }
    }
  } else if (info[0].As<String>() == String::New(env, "persistent")) {
    if (info[1].IsString()) {
      return persistent.Get(info[1].As<String>().Utf8Value());
    } else if (info[1].IsNumber()) {
      return persistent.Get(info[1].As<Number>().Uint32Value());
    }
  } else {
    if (info[0].IsString()) {
      return reference.Get(info[0].As<String>().Utf8Value());
    } else if (info[0].IsNumber()) {
      return reference.Get(info[0].As<Number>().Uint32Value());
    }
  }

  return String::New(env, "Error: Reached end of getter");
}

Value GetCastedFromValue(const CallbackInfo& info) {
  Env env = info.Env();

  if (info[0].As<String>() == String::New(env, "weak")) {
    if (casted_weak.IsEmpty()) {
      return String::New(env, "No Referenced Value");
    } else {
      return casted_weak.Value();
    }
  } else if (info[0].As<String>() == String::New(env, "persistent")) {
    return casted_persistent.Value();
  } else {
    return casted_reference.Value();
  }
}

Value GetCastedFromGetter(const CallbackInfo& info) {
  Env env = info.Env();

  if (info[0].As<String>() == String::New(env, "weak")) {
    if (casted_weak.IsEmpty()) {
      return String::New(env, "No Referenced Value");
    } else {
      return casted_weak.Get(info[1].As<Number>());
    }
  } else if (info[0].As<String>() == String::New(env, "persistent")) {
    return casted_persistent.Get(info[1].As<Number>());
  } else {
    return casted_reference.Get(info[1].As<Number>());
  }
}

Number UnrefObjects(const CallbackInfo& info) {
  Env env = info.Env();
  uint32_t num;

  if (info[0].As<String>() == String::New(env, "weak")) {
    num = weak.Unref();
  } else if (info[0].As<String>() == String::New(env, "persistent")) {
    num = persistent.Unref();
  } else if (info[0].As<String>() == String::New(env, "references")) {
    num = reference.Unref();
  } else if (info[0].As<String>() == String::New(env, "casted weak")) {
    num = casted_weak.Unref();
  } else if (info[0].As<String>() == String::New(env, "casted persistent")) {
    num = casted_persistent.Unref();
  } else {
    num = casted_reference.Unref();
  }

  return Number::New(env, num);
}

Number RefObjects(const CallbackInfo& info) {
  Env env = info.Env();
  uint32_t num;

  if (info[0].As<String>() == String::New(env, "weak")) {
    num = weak.Ref();
  } else if (info[0].As<String>() == String::New(env, "persistent")) {
    num = persistent.Ref();
  } else if (info[0].As<String>() == String::New(env, "references")) {
    num = reference.Ref();
  } else if (info[0].As<String>() == String::New(env, "casted weak")) {
    num = casted_weak.Ref();
  } else if (info[0].As<String>() == String::New(env, "casted persistent")) {
    num = casted_persistent.Ref();
  } else {
    num = casted_reference.Ref();
  }

  return Number::New(env, num);
}

Object InitObjectReference(Env env) {
  Object exports = Object::New(env);
  
  exports["setCastedObjects"] = Function::New(env, SetCastedObjects);
  exports["setObjects"] = Function::New(env, SetObjects);
  exports["getCastedFromValue"] = Function::New(env, GetCastedFromValue);
  exports["getFromGetter"] = Function::New(env, GetFromGetter);
  exports["getCastedFromGetter"] = Function::New(env, GetCastedFromGetter);
  exports["getFromValue"] = Function::New(env, GetFromValue);
  exports["unrefObjects"] = Function::New(env, UnrefObjects);
  exports["refObjects"] = Function::New(env, RefObjects);

  return exports;
}
