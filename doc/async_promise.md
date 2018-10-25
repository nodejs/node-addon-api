# AsyncPromise

The `Napi::AsyncPromise<T>` class along with its abstract `BaseTask` inner
class provide the ability to queue an asynchronous task which will resolve a promise
on successful completion or reject it on error. You can use these classes to handle
many of the the tedious tasks of signaling asynchronous completion and returning
results to the event loop from worker threads.

The `Napi::AsyncPromise<T>` class manages the initial lifetime of
an instance of the user-defined task `<T>` which must be a subclass of the
`BaseTask` inner class. It creates an instance during construction, and it will
destroy the instance and signal promise rejection in the event that the N-API
native function which uses it exits before the task is successfully queued.

The user must declare a subclass of `Napi::AsyncPromise<T>::BaseTask` and, at
minimum, implement a constructor and an override of the pure virtual
`Napi::AsyncPromise<T>::BaseTask::Execute()` method. The default implementation of
success handling will resolve the promise with the `undefined` JavaScript value.
The default implementation of error handling will reject the promise with a
JavaScript error value. The methods `Napi::AsyncPromise<T>::BaseTask::OnOK()` and
`Napi::AsyncPromise<T>::BaseTask::OnError()` may be overridden as needed to provide
other results to code awaiting promise fulfillment.

The user will often also wish to add other methods to the task subclass to
perform initialization needed for deferred task execution. The native N-API
function accesses the instance of the task subclass by calling
`Napi::AsyncPromise<T>::Task()`.

When the task is fully initialized, the N-API native function requests execution
by calling `Napi::AsyncPromise<T>::Queue()`. When a worker thread is available
the `<T>::Execute()` method will be invoked.  If `<T>::Execute()`
completes without throwing an exception or making a call to
`BaseTask::SetError()`, then `<T>::OnOK()` will be invoked on the event thread.
If `<T>::Execute()` instead throws an exception or calls `BaseTask::SetError()`,
`<T>::OnError()` will be invoked on the event thread. After one of these
completion methods has finished, the instance of `<T>` is destroyed.

## AsyncPromise\<T\>::BaseTask Types

### Base_t

Defines the fully-qualified type of `BaseTask` for convience.

## AsyncPromise\<T\>::BaseTask Public Methods

### Env

```cpp
Napi::Env Napi::AsyncPromise<T>::BaseTask::Env() const;
```

Requests the environment in which the task was created.

#### Arguments
None

## AsyncPromise\<T\>::BaseTask Public Operators

### napi_async_work

```cpp
Napi::AsyncPromise<T>::BaseTask::operator napi_async_work() const;
```

Returns the N-API `napi_async_work` wrapped by the `<T>` instance. This
can be used to mix usage of the C N-API and node-addon-api.

### napi_deferred

```cpp
Napi::AsyncPromise<T>::BaseTask::operator napi_deferred() const;
```

Returns the N-API `napi_deferred` wrapped by the `<T>` instance. This
can be used to mix usage of the C N-API and node-addon-api.

## AsyncPromise\<T\>::BaseTask Protected Methods

### Constructor

```cpp
explicit Napi::AsyncPromise<T>::BaseTask::BaseTask(Napi::Env env);
```

Constructs the `BaseTask` instance.

#### Arguments
- `[in] env`: The environment in which the task is being created

### SetError

```cpp
void Napi::AsyncPromise<T>::BaseTask::SetError(const std::string& error);
```

To be used from `<T>::Execute()`. Sets an error message for later use by
`<T>::OnError`. Calling this method will cause `<T>::OnError` to be
invoked after task execution is complete. The default implementation of
`BaseTask::OnError()` will reject the promise with a JavaScript error
instance having the message passed in the argument to this function.

#### Arguments
- `[in] error`: An error message associated with the promise rejection

### Resolve

```cpp
void Napi::AsyncPromise<T>::BaseTask::Resolve(Value value);
```

To be used from `<T>::OnOK()`. **Do not** use this method within `<T>::Execute()`.
The default implementation of `BaseTask::OnOK()` calls this method with the
JavaScript `undefined` value to resolve the promise.

#### Arguments
- `[in] value`: The value with which to resolve the promise

### Reject

```cpp
void Napi::AsyncPromise<T>::BaseTask::Reject(Value value) const;
```

To be used from `<T>::OnError()`. **Do not** use this method within `<T>::Execute()`.
The default implementation of `BaseTask::OnError()` calls this method with a
JavaScript error object to reject the promise.

#### Arguments
- `[in] value`: The value with which to reject the promise

### Execute

```cpp
virtual void Napi::AsyncPromise<T>::BaseTask::Execute() = 0;
```

The method called by the worker thread to perform the deferred task. If the
task completes successfully, store any values needed for promise resolution
in the `<T>` instance on which this method is executed. If the task fails,
call `BaseTask::SetError()` to record the error for promise rejection.

#### Arguments
None

### OnOK

```cpp
virtual void Napi::AsyncPromise<T>::BaseTask::OnOK();
```

The method is scheduled on the main event loop following
successfull completion of `<T>::Execute()`. The default implementation
resolves the promise with the JavaScript `undefined` value. Override this
method to perform custom promise resolution.

#### Arguments
None

### OnCancel

```cpp
virtual void Napi::AsyncPromise<T>::BaseTask::OnCancel();
```

The method is scheduled on the main event loop following
cancellation of the task. The task will only be canceled by a call to
`AsyncPromise<T>::Cancel()`. The default implementation rejects the promise with
a JavaScript string value. Override this method to perform a different operation
when the task is cancelled.

#### Arguments
None

### OnError

```cpp
virtual Napi::AsyncPromise<T>::BaseTask::OnError();
```

The method is scheduled on the main event loop following
unsuccessful completion of or an exception thrown from `<T>::Execute()`.
The default implementation rejects the promise with an instance of a
JavaScript error constructed with either the message passed to
`BaseTask::SetError()` or, if an exception was caught, the result of
calling `std::exception::what()`.

#### Arguments
None

## AsyncPromise\<T\> Methods

### Constructor

```cpp
explicit Napi::AsyncPromise<T>::AsyncPromise(Napi::Env env);
```

Constructs and initializes an instance of the `AsyncPromise` management class
for a task of type `<T>`.

#### Arguments
- `[in] env`: The environment in which the task is being created

### Constructor

```cpp
Napi::AsyncPromise<T>::AsyncPromise(Napi::Env env, const char* resource_name);
```

Constructs and initializes an instance of the `AsyncPromise` management class
for a task of type `<T>`.

#### Arguments
- `[in] env`: The environment in which the task is being created
- `[in] resource_name`: Identifier for the kind of resource that is being provided for diagnostic information exposed by the `async_hooks` API

### Constructor

```cpp
Napi::AsyncPromise<T>::AsyncPromise(Napi::Env env, const char* resource_name, const Object& resource);
```

Constructs and initializes an instance of the `AsyncPromise` management class
for a task of type `<T>`.

#### Arguments
- `[in] env`: The environment in which the task is being created
- `[in] resource_name`: Identifier for the kind of resource that is being provided for diagnostic information exposed by the `async_hooks` API
- `[in] resource`: An object associated with the async work that will be passed to possible `async_hooks` init hooks

### Constructor

```cpp
Napi::AsyncPromise<T>::AsyncPromise(AsyncPromise&& other);
```

Constructs and initializes an instance of the `AsyncPromise` management class
for a task of type `<T>` by taking ownership from another instance.

#### Arguments
- `[in] other`: The other instance from which to take ownership

### Env

```cpp
Napi::Env Napi::AsyncPromise<T>::Env() const;
```

Requests the environment in which the task was created.

#### Arguments
None

### Promise

```cpp
Napi::Promise Napi::AsyncPromise<T>::Promise() const;
```

Requests the promise associated with the task.

#### Arguments
None

### Task

```cpp
<T> * Napi::AsyncPromise<T>::Task();
```

Requests a pointer to the instance of the user-defined task that will be scheduled.
Note that after the task has been queued, access to task instance may result in
undefined behavior.

#### Arguments
None

### Queue

```cpp
void Napi::AsyncPromise<T>::Queue();
```

Submits the underlying task for processing on a worker thread. After the task has
been queued, any further access to the task instance may result in undefined
behavior. The only subsequent legal operations are request for cancellation and
the `Napi::Promise` object.

#### Arguments
None

### Cancel

```cpp
void Napi::AsyncPromise<T>::Cancel();
```

Attempts to cancel processing for the underlying task. Cancelation can only be
successful if the task has been queued but not yet assigned to a work queue
thread. If cancellation is
successful, `<T>::OnCancel()` will be called. Override that message in order
to provide custom cancelation handling.

#### Arguments
None

### Resolve

```cpp
void Napi::AsyncPromise<T>::Resolve(Value value);
```

Resolves the promise using the provided value. The native N-API routine may
want to do this in the event that the promise conditions can be resolved
without executing the task. If the task has been queued or already filfilled,
this method will throw a JavaScript error.

#### Arguments
- `[in] value`: The value with which to resolve the promise

### Reject

```cpp
void Napi::AsyncPromise<T>::Reject(Value value);
```

Rejects the promise using the provided value. The native N-API routine may
want to do this in the event that the promise conditions can be rejected
without executing the task - for instance, during parameter validation.
If the task has been queued or already filfilled,
this method will throw a JavaScript error.

#### Arguments
- `[in] value`: The value with which to resolve the promise

## AsyncPromise\<T\> Operators

### napi_async_work

```cpp
Napi::AsyncPromise<T>::operator napi_async_work() const;
```

Returns the N-API `napi_async_work` wrapped by the `<T>` instance. This
can be used to mix usage of the C N-API and node-addon-api.

### napi_deferred

```cpp
Napi::AsyncPromise<T>::operator napi_deferred() const;
```

Returns the N-API `napi_deferred` wrapped by the `<T>` instance. This
can be used to mix usage of the C N-API and node-addon-api.

## Example

The basic usage pattern for `Napi::AsyncPromise` is summarized in these steps:

1. Declare a subclass of `Napi::AsyncPromise<T>::BaseTask` using the subclass
type as the template parameter
2. In the subclass, define a constructor and override the `Execute()` abstract method
3. At the top of the N-API native routine, declare an instance of `Napi::AsyncPromise<T>`
using the subclass type as the template parameter
4. Set the subclass task parameters
5. Queue the async promise
6. Return the promise from the native routine

As noted above, the the other virtual methods of `Napi::AsyncPromis<T>::BaseTask`
may be overridden to provide custom behavior.

The code below demonstrates a basic example of using `Napi::AsyncPromise`:

```cpp
#include <napi.h>

#include <chrono>
#include <thread>

// Native routine registered with N-API as a JavaScript callable function
Napi::Value PromisingEcho(const Napi::CallbackInfo & info) {
  // Define the task subclass
  class EchoTask : public Napi::AsyncPromise<EchoTask>::BaseTask {
    private:
      std::string _echo_str;

      void Execute() override {
        // Simulate an asynchronous task running on a worker thread
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // Set data for promise fulfillment
        _echo_str += " ... done";
      }

      void OnOK() override {
        // Echo the message to any awaiters
        Resolve(Napi::String::New(Env(), _echo_str));
      }

    public:
      EchoTask(Napi::Env env) : Base_t(env) {}

      void SetString(const std::string & echo_str) {
        // Save this for later
        _echo_str = echo_str;
      }
  }

  // Declare the async promise that we'll be returning
  AsyncPromise<EchoTask> echo_promise(info.Env());

  // Validate the function parameter from our JavaScript caller
  if (info.Length() < 1 || !info[0].IsString()) {
    // Bad argument - reject the promise and bail without queueing the task
    echo_promise.Reject(Napi::TypeError::New(info.Env(), "Echo requires a string argument").Value());
    return echo_promise.Promise();
  }

  // Set the task parameter
  echo_promise.Task()->SetString(info[0].As<Napi::String>());

  // Queue the async work
  echo_promise.Queue();

  // Return the promise to JavaScript land
  return echo_promise.Promise();
}
```
