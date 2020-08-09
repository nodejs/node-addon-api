# ThreadSafeFunctionEx

The `Napi::ThreadSafeFunctionEx` type provides APIs for threads to communicate
with the addon's main thread to invoke JavaScript functions on their behalf. The
type is a three-argument templated class, each argument representing the type
of:
- `ContextType = std::nullptr_t`: The thread-safe function's context. By default,
  a TSFN has no context.
- `DataType = void*`: The data to use in the native callback. By default, a TSFN
  can accept *any* data type.
- `Callback = void(*)(Napi::Env, Napi::Function jsCallback, ContextType*,
  DataType*)`: The callback to run for each item added to the queue. If no
  `Callback` is given, the API will call the function `jsCallback` with no
  arguments.

Documentation can be found for an [overview of the API](threadsafe.md), as well
as [differences between the two thread-safe function
APIs](threadsafe.md#implementation-differences).

## Methods

### Constructor

Creates a new empty instance of `Napi::ThreadSafeFunctionEx`.

```cpp
Napi::Function::ThreadSafeFunctionEx<ContextType, DataType, Callback>::ThreadSafeFunctionEx();
```

### Constructor

Creates a new instance of the `Napi::ThreadSafeFunctionEx` object.

```cpp
Napi::ThreadSafeFunctionEx<ContextType, DataType, Callback>::ThreadSafeFunctionEx(napi_threadsafe_function tsfn);
```

- `tsfn`: The `napi_threadsafe_function` which is a handle for an existing
  thread-safe function.

Returns a non-empty `Napi::ThreadSafeFunctionEx` instance. To ensure the API
statically handles the correct return type for `GetContext()` and
`[Non]BlockingCall()`, pass the proper template arguments to
`Napi::ThreadSafeFunctionEx`.

### New

Creates a new instance of the `Napi::ThreadSafeFunctionEx` object. The `New`
function has several overloads for the various optional parameters: skip the
optional parameter for that specific overload.

```cpp
New(napi_env env,
    CallbackType callback,
    const Object& resource,
    ResourceString resourceName,
    size_t maxQueueSize,
    size_t initialThreadCount,
    ContextType* context,
    Finalizer finalizeCallback,
    FinalizerDataType* data = nullptr);
```

- `env`: The `napi_env` environment in which to construct the
  `Napi::ThreadSafeFunction` object.
- `[optional] callback`: The `Function` to call from another thread.
- `[optional] resource`: An object associated with the async work that will be
  passed to possible async_hooks init hooks.
- `resourceName`: A JavaScript string to provide an identifier for the kind of
  resource that is being provided for diagnostic information exposed by the
  async_hooks API.
- `maxQueueSize`: Maximum size of the queue. `0` for no limit.
- `initialThreadCount`: The initial number of threads, including the main
  thread, which will be making use of this function.
- `[optional] context`: Data to attach to the resulting `ThreadSafeFunction`.
  It can be retreived via `GetContext()`.
- `[optional] finalizeCallback`: Function to call when the
  `ThreadSafeFunctionEx` is being destroyed.  This callback will be invoked on
  the main thread when the thread-safe function is about to be destroyed. It
  receives the context and the finalize data given during construction (if
  given), and provides an opportunity for cleaning up after the threads e.g. by
  calling `uv_thread_join()`. It is important that, aside from the main loop
  thread, there be no threads left using the thread-safe function after the
  finalize callback completes. Must implement `void operator()(Env env,
  DataType* data, ContextType* hint)`.
- `[optional] data`: Data to be passed to `finalizeCallback`.

Returns a non-empty `Napi::ThreadSafeFunctionEx` instance.

Depending on the targetted `NAPI_VERSION`, the API has different implementations
for `CallbackType callback`.

When targetting version 4, `callback` may be:
- of type `const Function&`
- not provided as an parameter, in which case the API creates a new no-op
`Function`

When targetting version 5+, `callback` may be:
- of type `const Function&`
- of type `std::nullptr_t`
- not provided as an parameter, in which case the API passes `std::nullptr`

### Acquire

Add a thread to this thread-safe function object, indicating that a new thread
will start making use of the thread-safe function.

```cpp
napi_status Napi::ThreadSafeFunctionEx<ContextType, DataType, Callback>::Acquire()
```

Returns one of:
- `napi_ok`: The thread has successfully acquired the thread-safe function for
  its use.
- `napi_closing`: The thread-safe function has been marked as closing via a
  previous call to `Abort()`.

### Release

Indicate that an existing thread will stop making use of the thread-safe
function. A thread should call this API when it stops making use of this
thread-safe function. Using any thread-safe APIs after having called this API
has undefined results in the current thread, as it may have been destroyed.

```cpp
napi_status Napi::ThreadSafeFunctionEx<ContextType, DataType, Callback>::Release()
```

Returns one of:
- `napi_ok`: The thread-safe function has been successfully released.
- `napi_invalid_arg`: The thread-safe function's thread-count is zero.
- `napi_generic_failure`: A generic error occurred when attemping to release the
  thread-safe function.

### Abort

"Abort" the thread-safe function. This will cause all subsequent APIs associated
with the thread-safe function except `Release()` to return `napi_closing` even
before its reference count reaches zero. In particular, `BlockingCall` and
`NonBlockingCall()` will return `napi_closing`, thus informing the threads that
it is no longer possible to make asynchronous calls to the thread-safe function.
This can be used as a criterion for terminating the thread. Upon receiving a
return value of `napi_closing` from a thread-safe function call a thread must
make no further use of the thread-safe function because it is no longer
guaranteed to be allocated.

```cpp
napi_status Napi::ThreadSafeFunctionEx<ContextType, DataType, Callback>::Abort()
```

Returns one of:
- `napi_ok`: The thread-safe function has been successfully aborted.
- `napi_invalid_arg`: The thread-safe function's thread-count is zero.
- `napi_generic_failure`: A generic error occurred when attemping to abort the
  thread-safe function.

### BlockingCall / NonBlockingCall

Calls the Javascript function in either a blocking or non-blocking fashion.
- `BlockingCall()`: the API blocks until space becomes available in the queue.
  Will never block if the thread-safe function was created with a maximum queue
  size of `0`.
- `NonBlockingCall()`: will return `napi_queue_full` if the queue was full,
  preventing data from being successfully added to the queue.

```cpp
napi_status Napi::ThreadSafeFunctionEx<ContextType, DataType, Callback>::BlockingCall(DataType* data = nullptr) const

napi_status Napi::ThreadSafeFunctionEx<ContextType, DataType, Callback>::NonBlockingCall(DataType* data = nullptr) const
```

- `[optional] data`: Data to pass to the callback which was passed to
  `ThreadSafeFunctionEx::New()`.

Returns one of:
- `napi_ok`: The call was successfully added to the queue.
- `napi_queue_full`: The queue was full when trying to call in a non-blocking
  method.
- `napi_closing`: The thread-safe function is aborted and cannot accept more
  calls.
- `napi_invalid_arg`: The thread-safe function is closed.
- `napi_generic_failure`: A generic error occurred when attemping to add to the
  queue.

## Example

For an in-line documented example, please see the ThreadSafeFunctionEx CI tests hosted here.
- [test/threadsafe_function_ex/test/example.js](../test/threadsafe_function_ex/test/example.js)
- [test/threadsafe_function_ex/test/example.cc](../test/threadsafe_function_ex/test/example.cc)

The example will create multiple set of threads. Each thread calls into
JavaScript with a numeric `base` value (deterministically calculated by the
thread id), with Node returning either a `number` or `Promise<number>` that
resolves to `base * base`.

From the root of the `node-addon-api` repository:

```
Usage: node ./test/threadsafe_function_ex/test/example.js [options]

      -c, --calls <calls>                   The number of calls each thread should make (number[]).
      -a, --acquire [factor]                Acquire a new set of `factor` call threads, using the
                                            same `calls` definition.
      -d, --call-delay <call-delays>        The delay on callback resolution that each thread should
                                            have (number[]). This is achieved via a delayed Promise
                                            resolution in the JavaScript callback provided to the
                                            TSFN. Using large delays here will cause all threads to
                                            bottle-neck.
      -D, --thread-delay <thread-delays>    The delay that each thread should have prior to making a
                                            call (number[]). Using large delays here will cause the
                                            individual thread to bottle-neck.
      -l, --log-call                        Display console.log-based logging messages.
      -L, --log-thread                      Display std::cout-based logging messages.
      -n, --no-callback                     Do not use a JavaScript callback.
      -e, --callback-error [thread[.call]]  Cause an error to occur in the JavaScript callback for
                                            the given thread's call (if provided; first thread's
                                            first call otherwise).

  When not provided:
      - <calls> defaults to [1,2,3,4,5]
      - [factor] defaults to 1
      - <call-delays> defaults to [400,200,100,50,0]
      - <thread-delays> defaults to [400,200,100,50,0]


Examples:

      -c [1,2,3] -l -L

          Creates three threads that makes one, two, and three calls each, respectively.

      -c [5,5] -d [5000,5000] -D [0,0] -l -L

          Creates two threads that make five calls each. In this scenario, the threads will be
          blocked primarily on waiting for the callback to resolve, as each thread's call takes
          5000 milliseconds.
```
