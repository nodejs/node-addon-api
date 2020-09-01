#GenericCallbackWrapper

`Napi::GenericCallbackWrapper<T>` is a class with one template parameter `T`, 
which is intended to allow you to return `T` from a C++ function running arbitrary async i.e. concurrency 
regimes like `boost::asio` via a `Napi::Promise` i.e. JS promise, including exception and conversion function handling. 

All you need is the `Napi::Promise::Deferred` you want to Resolve/Reject, a function to convert your type `T` 
to a `Napi::Value` to construct your wrapper, which then can give you a `std::function` callback, which
can be called later from any custom thread, in order to resolve/reject the promise. 
Life time handling included. That means that the wrapper is internally reference counted and don't need to be stored, 
the function object you get from `get_native_callback` will ensure the lifetime.

##Self managed thread example

```cpp
value_t InsertYourNativeFunctionHere(value_t input) {
    if (input == 42u) {
        throw std::runtime_error("Throwing on 42");//Throwing is ok and will be in the JS promise
    }
    return input * input * input;
}

void start_task(value_t input, std::function<void(std::future<value_t>)> completion_handler) {
    std::packaged_task<value_t()> task
    {std::bind(&InsertYourNativeFunctionHere, input)};

    auto t = std::thread([task = std::move(task), completion_handler]() mutable {
        task();
        completion_handler(task.get_future());
    });
    t.detach();
}

/*!
 * @returns A promise resolved with a number or with any exception thrown from the native code
 */
Napi::Value JSFrontingNativeFunction(const Napi::CallbackInfo& info){
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(info.Env());

    auto wrapper = Napi::GenericCallbackWrapper<value_t>
            (deferred,[](const auto &env, auto &&future) -> Napi::Value {
                return Napi::Number::New(env, future.get());
            });

    start_task(info[0].As<Napi::Number>().Uint32Value(), wrapper.get_native_callback());
    return deferred.Promise();
}
}
```

##Boost ASIO
As in the mentioned example above, all you have to do is to replace the thread generation with 
either `boost::asio::dispatch` or `boost::asio::post` like:

```cpp
void start_task(value_t input, std::function<void(std::future<value_t>)> completion_handler) {
    std::packaged_task<value_t(value_t)> task
    {std::bind(&InsertYourNativeFunctionHere, input)};

    boost::asio::dispatch(
         [task = std::move(
                 task), input, completion_handler]() mutable {
             task(input);
            /*the completion_handler could also be called directly 
             * here but lets assume you are running on a congested strand so you just post the lengthly js completion*/
             boost::asio::post(
                     [future = task.get_future(), completion_handler]() mutable {
                         completion_handler(std::move(future));
                     }
            );
    });
}
```

 