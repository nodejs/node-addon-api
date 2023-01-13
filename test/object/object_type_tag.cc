#include "napi.h"

#if (NAPI_VERSION > 7)

using namespace Napi;

static const napi_type_tag type_tags[5] = {
    {0xdaf987b3cc62481a, 0xb745b0497f299531},
    {0xbb7936c374084d9b, 0xa9548d0762eeedb9},
    {0xa5ed9ce2e4c00c38, 0},
    {0, 0},
    {0xa5ed9ce2e4c00c38, 0xdaf987b3cc62481a},
};

Value TypeTaggedInstance(const CallbackInfo& info) {
  Object instance = Object::New(info.Env());
  uint32_t type_index = info[0].As<Number>().Int32Value();

  instance.TypeTag(&type_tags[type_index]);

  return instance;
}

Value CheckTypeTag(const CallbackInfo& info) {
  uint32_t type_index = info[0].As<Number>().Int32Value();
  Object instance = info[1].As<Object>();

  return Boolean::New(info.Env(),
                      instance.CheckTypeTag(&type_tags[type_index]));
}

Object InitObjectTypeTag(Env env) {
  Object exports = Object::New(env);
  exports["checkTypeTag"] = Function::New(env, CheckTypeTag);
  exports["typedTaggedInstance"] = Function::New(env, TypeTaggedInstance);
  return exports;
}

#endif
