# Thread-safe Functions

JavaScript functions can normally only be called from a native addon's main
thread. If an addon creates additional threads, then node-addon-api functions
that require a `Napi::Env`, `Napi::Value`, or `Napi::Reference` must not be
called from those threads.

When an addon has additional threads and JavaScript functions need to be invoked
based on the processing completed by those threads, those threads must
communicate with the addon's main thread so that the main thread can invoke the
JavaScript function on their behalf. The thread-safe function APIs provide an
easy way to do this. These APIs provide two types --
[`Napi::ThreadSafeFunction`](threadsafe_function.md) and
[`Napi::ThreadSafeFunctionEx`](threadsafe_function_ex.md) -- as well as APIs to
create, destroy, and call objects of this type. The differences between the two
are subtle and are [highlighted below](#implementation-differences). Regardless
of which type you choose, the API between the two are similar.

`Napi::ThreadSafeFunction[Ex]::New()` creates a persistent reference that holds
a JavaScript function which can be called from multiple threads. The calls
happen asynchronously. This means that values with which the JavaScript callback
is to be called will be placed in a queue, and, for each value in the queue, a
call will eventually be made to the JavaScript function.

`Napi::ThreadSafeFunction[Ex]` objects are destroyed when every thread which
uses the object has called `Release()` or has received a return status of
`napi_closing` in response to a call to `BlockingCall()` or `NonBlockingCall()`.
The queue is emptied before the `Napi::ThreadSafeFunction[Ex]` is destroyed. It
is important that `Release()` be the last API call made in conjunction with a
given `Napi::ThreadSafeFunction[Ex]`, because after the call completes, there is
no guarantee that the `Napi::ThreadSafeFunction[Ex]` is still allocated. For the
same reason it is also important that no more use be made of a thread-safe
function after receiving a return value of `napi_closing` in response to a call
to `BlockingCall()` or `NonBlockingCall()`. Data associated with the
`Napi::ThreadSafeFunction[Ex]` can be freed in its `Finalizer` callback which
was passed to `ThreadSafeFunction[Ex]::New()`.

Once the number of threads making use of a `Napi::ThreadSafeFunction[Ex]`
reaches zero, no further threads can start making use of it by calling
`Acquire()`. In fact, all subsequent API calls associated with it, except
`Release()`, will return an error value of `napi_closing`.

## Implementation Differences

The choice between `Napi::ThreadSafeFunction` and `Napi::ThreadSafeFunctionEx`
depends largely on how you plan to execute your native C++ code (the "callback")
on the Node thread.

### [`Napi::ThreadSafeFunction`](threadsafe_function.md)

This API is designed without N-API 5 native support for [optional JavaScript
  function callback feature](https://github.com/nodejs/node/commit/53297e66cb).
  `::New` methods that do not have a `Function` parameter will construct a
  _new_, no-op `Function` on the environment to pass to the underlying N-API
  call.

This API has some dynamic functionality, in that:
- The `[Non]BlockingCall()` methods provide a `Napi::Function` parameter as the
  callback to run when processing the data item on the main thread -- the
  `CallJs` callback. Since the callback is a parameter, it can be changed for
  every call.
- Different C++ data types may be passed with each call of `[Non]BlockingCall()`
  to match the specific data type as specified in the `CallJs` callback.

However, this functionality comes with some **additional overhead** and
situational **memory leaks**:
- The API acts as a "middle-man" between the underlying
  `napi_threadsafe_function`, and dynamically constructs a wrapper for your
  callback on the heap for every call to `[Non]BlockingCall()`.
- In acting in this "middle-man" fashion, the API will call the underlying "make
  call" N-API method on this packaged item. If the API has determined the
  threadsafe function is no longer accessible (eg. all threads have Released yet
  there are still items on the queue), **the callback passed to
  [Non]BlockingCall will not execute**. This means it is impossible to perform
  clean-up for calls that never execute their `CallJs` callback. **This may lead
  to memory leaks** if you are dynamically allocating memory.
- The `CallJs` does not receive the threadsafe function's context as a
  parameter. In order for the callback to access the context, it must have a
  reference to either (1) the context directly, or (2) the threadsafe function
  to call `GetContext()`. Furthermore, the `GetContext()` method is not
  _type-safe_, as the method returns an object that can be "any-casted", instead
  of having a static type.

### [`Napi::ThreadSafeFunctionEx`](threadsafe_function_ex.md)

The `ThreadSafeFunctionEx` class is a new implementation to address the
drawbacks listed above. The API is designed with N-API 5's support of an
optional function callback. The API will correctly allow developers to pass
`std::nullptr` instead of a `const Function&` for the callback function
specified in `::New`. It also provides helper APIs to _target_ N-API 4 and
construct a no-op `Function` **or** to target N-API 5 and "construct" an
`std::nullptr` callback. This allows a single codebase to use the same APIs,
with just a switch of the `NAPI_VERSION` compile-time constant.

The removal of the dynamic call functionality has the additional side effects:
- The API does _not_ act as a "middle-man" compared to the non-`Ex`. Once Node
  finalizes the threadsafe function, the `CallJs` callback will execute with an
  empty `Napi::Env` for any remaining items on the queue. This provides the the
  ability to handle any necessary clean up of the item's data.
- The callback _does_ receive the context as a parameter, so a call to
  `GetContext()` is _not_ necessary. This context type is specified as the
  **first type argument** specified to `::New`, ensuring type safety.
- The `New()` constructor accepts the `CallJs` callback as the **second type
  argument**. The callback must be statically defined for the API to access it.
  This affords the ability to statically pass the context as the correct type
  across all methods.
- Only one C++ data type may be specified to every call to `[Non]BlockingCall()`
  -- the **third type argument** specified to `::New`. Any "dynamic call data"
  must be implemented by the user.


### Usage Suggestions

In summary, it may be best to use `Napi::ThreadSafeFunctionEx` if:

- static, compile-time support for targeting N-API 4 or 5+ with an optional
  JavaScript callback feature is desired;
- the callback can have `static` storage class and will not change across calls
  to `[Non]BlockingCall()`;
- cleanup of items' data is required (eg. deleting dynamically-allocated data
  that is created at the caller level).

Otherwise, `Napi::ThreadSafeFunction` may be a better choice.
