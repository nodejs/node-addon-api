#GenericCallbackWrapper

`Napi::GenericCallbackWrapper<T>` is a class with one template parameter `T`, 
which is intended to allow you to return `T` from a C++ function running arbitrary async i.e. concurrency 
regimes via a `Napi::Promise` i.e. JS promise, including exception and conversion function handling. 

All you need is the `Napi::Promise::Deferred` you want to Resolve/Reject, a function to convert your type `T` 
to a `Napi::Value` to construct your wrapper, which then can give you a `std::function` callback, which
can be called later from any custom thread, in order to resolve/reject the promise. 
Life time handling included. 

##Example

```cpp
Napi::Promise MyNapiPowFunction(const Napi::CallbackInfo &info) {
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(info.Env());

    auto wrapper = generic_callback_wrapper<unsigned>
            (deferred, &convert_result_t);


TODO


    return deferred.Promise();
}

```

 