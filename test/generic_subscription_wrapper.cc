#include "napi.h"

#include <thread>
#include <stdexcept>
#include <cstdint>
#include <future>
#include <utility>
#include <functional>

#if (NAPI_VERSION > 3)

#ifdef NAPI_CPP_EXCEPTIONS

namespace {


}

Napi::Object InitGenericSubscriptionWrapper(Napi::Env env) {
    using namespace Napi;
    Object exports = Object::New(env);


    return exports;
}

#else

Napi::Object InitGenericSubscriptionWrapper(Napi::Env env) {
  return Napi::Object::New(env);
}

#endif

#endif
