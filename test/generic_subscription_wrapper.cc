#include "napi.h"

#include <thread>
#include <stdexcept>
#include <cstdint>
#include <future>
#include <utility>
#include <functional>
#include <atomic>

#if (NAPI_VERSION > 3)

#ifdef NAPI_CPP_EXCEPTIONS

namespace {

Napi::Value convert_int(const Napi::Env &env, const std::shared_future<std::uint32_t> & future) {
    return Napi::Number::From(env, future.get());
}

Napi::Value SubScribeFor10Calls(const Napi::CallbackInfo& info) {
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(info.Env());

    auto wrapper = Napi::GenericSubscriptionWrapper<std::uint32_t>(
            deferred, &convert_int, info[0].As<Napi::Function>());


    auto t = std::thread([wrapper = std::move(wrapper)]() mutable {
        std::atomic<bool> run{true};
        wrapper.set_unsubscription_function([&run]() {
            run = false;
        });
        for (std::uint32_t counter{}; run; ++counter) {
            auto future = std::async(std::launch::deferred, [counter]() {
               return counter;
            });
            wrapper.get_native_callback()(future.share());
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    });
    t.detach();

    return deferred.Promise();
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
