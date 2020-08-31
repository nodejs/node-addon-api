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

using value_t = std::uint32_t;


value_t Pow3WithThrowAt42(value_t input) {
    if (input == 42u) {
        throw std::runtime_error("Throwing on 42");
    }
    return input * input * input;
}


Napi::Object ConvertPow3InputWithErrorValue(const Napi::Env &env,
                          std::future<value_t> &&value_future) {
    Napi::Object value = Napi::Object::New(env);
    try {
        value.Set("value", value_future.get());
    } catch(std::exception& e) {
        value.Set("error", Napi::Error::New(env, e.what()).Value());
    }
    return value;
}

void thread_main(value_t value, Napi::GenericCallbackWrapper<value_t>&& wrapper) {
    std::packaged_task<value_t()> task
    {std::bind(&Pow3WithThrowAt42, value)};

    auto t = std::thread([task = std::move(task), wrapper = std::move(wrapper)]() mutable {
        task();
        wrapper.get_native_callback()(task.get_future());
    });
    t.detach();
}


Napi::Value TestWithValuePow3(const Napi::CallbackInfo& info){
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(info.Env());

    auto wrapper = Napi::GenericCallbackWrapper<value_t>
            (deferred, &ConvertPow3InputWithErrorValue);

    thread_main(info[0].As<Napi::Number>().Uint32Value(), std::move(wrapper));
    return deferred.Promise();
}

}

Napi::Object InitGenericCallbackWrapper(Napi::Env env) {
  using namespace Napi;
  Object exports = Object::New(env);
  exports["TestWithValuePow3"] = Function::New(env, TestWithValuePow3);

  return exports;
}

#else

Napi::Object InitGenericCallbackWrapper(Napi::Env env) {
  return Napi::Object::New(env);
}

#endif

#endif
