# GenericSubscriptionWrapper

`Napi::GenericSubscriptionWrapper<T>` is a class with one template parameter `T`, 
which is intended to allow you to call a JavaScript callback with `T` as parameter repeatedly  
from a C++ function (subscribe) running arbitrary async i.e. concurrency/timer regimes like `boost::asio` via a `Napi::Promise`
until a callback in JavaScript is called (unsubscribe).

Overall this Object is quite similar to [GenericCallbackWrapper](generic_callback_wrapper.md) but intended to be used
for efficient repeatedly data push pattern, for example pushing device states triggered by arbitrary timers to maybe
multiple javascript callbacks. To give satisfying freedom for such house keeping, the unsubscribe function 
is also promised, for cases where the according native facilities has also to be looked up or messaged first.

`Napi::GenericSubscriptionWrapper<T>` requires C++ exceptions enabled, so works by default.

## Self managed thread example

```cpp
Napi::Value convert_int(const Napi::Env &env, const std::shared_future<std::uint32_t> & future) {
    return Napi::Number::From(env, future.get());
}

std::uint32_t pull_some_device_state();

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
            auto future = std::async(std::launch::deferred, []() {
               return pull_some_device_state();
            });
            wrapper.get_native_callback()(future.share());
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    });
    t.detach();

    return deferred.Promise();
}
```
