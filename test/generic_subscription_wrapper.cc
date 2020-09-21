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

Napi::Value SubScribeFor10Calls(const Napi::CallbackInfo&) {

    return {};
}


}

Napi::Object InitGenericSubscriptionWrapper(Napi::Env env) {
    using namespace Napi;
    Object exports = Object::New(env);
    exports["SubScribeFor10Calls"] = Function::New(env, SubScribeFor10Calls);

    return exports;
}

#else

Napi::Object InitGenericSubscriptionWrapper(Napi::Env env) {
  return Napi::Object::New(env);
}

#endif

#endif
