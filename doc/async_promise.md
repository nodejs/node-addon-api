# AsyncPromise

The `Napi::AsyncPromise` class along with its abstract `BaseTask` inner
class provide the ability to queue an asynchronous task which will resolve a promise
on successful completion or reject it on error. You can use these classes to handle
many of the the tedious tasks of signaling asynchronous completion and returning
results to the event loop from worker threads.

The `Napi::AsyncPromise` class manages the initial lifetime of an instance of a user-defined task. It creates an instance and arranges for its destruction. It
also provides access to the promise associated with the task.

There are three requirements for the user-defined task:
- It must be a subclass of `Napi::AsyncPromise::BaseTask`
- It must be default constructable
- It must override `Napi::AsyncPromise::BaseTask::Execute()`

There are additional methods which may be overridden to provide custom behavior:
- `Napi::AsyncPromise::BaseTask::OnInit()` will usually be overridden to provide
initialization of the task. If control passes out of this method without
fulfilling the promise, the task will be queued for execution.

  There are two overloads of this method:
  1. `OnInit()` taking no arguments
  2. `OnInit(const Napi::CallbackInfo &)`

  If an instance of `Napi::Env` is passed to the `Napi::AsyncPromise` factory
  method, the first overload will be called. If an instance of `Napi::CallbackInfo`
  is passed to the `Napi::AsyncPromise` factory method, the second overload will
  be called.

  **NOTE: Make sure that you implement the appropriate overload.**

  The default implementations of both methods do nothing. Customarily, the
  user will provide an implementation of the second overload in order to process
  arguments from JavaScript callers. If an error occurrs during initialization,
  call `Reject()` on the task before returning. The promise will be rejected,
  and the task will be destroyed without being queued for execution.

- `Napi::AsyncPromise::BaseTask::OnOK()` will usually be overridden to return
results to the code awaiting the promise resolution. This method will be called
if the override of `Execute()` completes without an exception and without calling
`SetError()`.

  The default implmentation will reslove the promise with the JavaScript
  `undefined` value. Override this method to provide custom results to the
  routine awaiting the task completion

- `Napi::AsyncPromise::BaseTask::OnCancel()` is called if the task is canceled
prior to being picked up by a thread in the work queue. A task that has begun
processing cannot be canceled.

  The default implementation will reject the promise with a message string.
  Override this method to provide custom behavior.

- `Napi::AsyncPromise::BaseTask::OnError()` is called if the the override of
`Execute()` either calls `SetError()` or throws an exception.

  The default implementation will reject the promise with a JavaScript error
  with the message provided either by the call to `SetError()` or by the
  exception that was thrown. Override this method to provide custom behavior.

The task instance will be destroyed when it is no longer needed. If
the promise was fulfilled without queuing the task, the task will be
destroyed when the parent instance of `Napi::AsyncPromise` goes out-of-scope.
If the task was queued for execution, the task will be destroyed after
execution or cancellation.

## AsyncPromise::BaseTask Types

### Base_t

Defines the fully-qualified type of `BaseTask` for convience.

## AsyncPromise::BaseTask Public Methods

### Env

```cpp
Napi::Env Napi::AsyncPromise::BaseTask::Env() const;
```

Requests the environment in which the task was created.

#### Arguments
None

## AsyncPromise::BaseTask Public Operators

### napi_async_work

```cpp
Napi::AsyncPromise::BaseTask::operator napi_async_work() const;
```

Returns the N-API `napi_async_work` wrapped by the task instance. This
can be used to mix usage of the C N-API and node-addon-api.

### napi_deferred

```cpp
Napi::AsyncPromise::BaseTask::operator napi_deferred() const;
```

Returns the N-API `napi_deferred` wrapped by the task instance. This
can be used to mix usage of the C N-API and node-addon-api.

## AsyncPromise::BaseTask Protected Methods

### Constructor

```cpp
explicit Napi::AsyncPromise::BaseTask::BaseTask();
```

Constructs the `BaseTask` instance.

#### Arguments
None


### SetError

```cpp
void Napi::AsyncPromise::BaseTask::SetError(const std::string& error);
```

To be used from `Execute()`. Sets an error message for later use by
`OnError()`. Calling this method will cause `OnError()` to be
invoked after task execution is complete.

The default implementation of
`BaseTask::OnError()` will reject the promise with a JavaScript error
instance having the message passed in the argument to this function.

#### Arguments
- `[in] error`: An error message associated with the promise rejection


### Resolve

```cpp
void Napi::AsyncPromise::BaseTask::Resolve(Value value);
```

To be used from `OnInit()`, `OnOK()`, `OnCancel()`, or `OnError()`.
**Do not** use this method from your implementation of `Execute()`.

The default implementation of `OnOK()` calls this method with the
JavaScript `undefined` value to resolve the promise.

#### Arguments
- `[in] value`: The value with which to resolve the promise


### Reject

```cpp
void Napi::AsyncPromise<T>::BaseTask::Reject(Value value) const;
```

To be used from `OnInit()`, `OnOK()`, `OnCancel()`, or `OnError()`.
**Do not** use this method from your implementation of `Execute()`.

The default implementation of `OnError()` calls this method with
a JavaScript error object to reject the promise.
The default implementation of `OnCancel()` calls this method with
a JavaScript message string indicating cancellation of the task.

#### Arguments
- `[in] value`: The value with which to reject the promise


### OnInit

```cpp
virtual void Napi::AsyncPromise<T>::BaseTask::OnInit();
```

This method is called after the base class initialization if only
the JavaScript environment was passed to the `Napi::AsyncPromise`
factory method.

The default implementation does nothing. If control passes out of
this method without fulfilling the promise, the task will be queued
for execution.

#### Arguments
None


### OnInit

```cpp
virtual void Napi::AsyncPromise<T>::BaseTask::OnInit(const CallbackInfo& info);
```

This method is called after the base class initialization if an instance
of `Napi::CallbackInfo` was passed to the `Napi::AsyncPromise`
factory method.

The default implementation does nothing. If control passes out of
this method without fulfilling the promise, the task will be queued
for execution.

#### Arguments
- `[in] info`: The arguments to the N-API callback function


### Execute

```cpp
virtual void Napi::AsyncPromise<T>::BaseTask::Execute() = 0;
```

This method called in the context of a worker thread to perform the deferred
task. Store any values needed for promise resolution by the result handlers
`OnOK()` and `OnError()`.
If the task fails, call `BaseTask::SetError()` to record the error and
trigger the calling of `OnError()`.

**NOTE: Do not call N-API functions from `Execute()`. Any such calls must
be made from result handlers.**

#### Arguments
None


### OnOK

```cpp
virtual void Napi::AsyncPromise<T>::BaseTask::OnOK();
```

This method is executed on the main event loop following
successfull completion of `Execute()`. N-API functions are available
to the implementation of this method.

The default implementation resolves the promise with the JavaScript
`undefined` value. Override this method to perform custom promise
resolution.

#### Arguments
None


### OnCancel

```cpp
virtual void Napi::AsyncPromise<T>::BaseTask::OnCancel();
```

This method is executed on the main event loop following
cancellation of the task. N-API functions are available to the
implementation of this method.

The task will only be canceled by a call to `AsyncPromise::Cancel()`.

The default implementation rejects the promise with
a JavaScript string value. Override this method to perform custom actions
when the task is cancelled.

#### Arguments
None


### OnError

```cpp
virtual Napi::AsyncPromise<T>::BaseTask::OnError();
```

This method is executed on the main event loop following
unsuccessful completion of or an exception thrown from `Execute()`.
N-API functions are available to the implementation of this method.

The default implementation rejects the promise with an instance of a
JavaScript error constructed with either the message passed to
`SetError()` or, if an exception was caught, the result of
calling `std::exception::what()`.

#### Arguments
None

## AsyncPromise Methods

### Factory

```cpp
static Napi::AsyncPromise AsyncPromise::New<T>(Napi::Env env);
```

Constructs and initializes an instance of `AsyncPromise`
for a task of type `<T>`.

When this factory method is used, the task initialization handler `OnInit()`
with no arguments will be invoked.

#### Arguments
- `[in] env`: The environment in which the task is being created


### Factory

```cpp
static Napi::AsyncPromise AsyncPromise::New<T>(Napi::Env env, const char* resource_name);
```

Constructs and initializes an instance of `AsyncPromise`
for a task of type `<T>`.

When this factory method is used, the task initialization handler `OnInit()`
with no arguments will be invoked.

#### Arguments
- `[in] env`: The environment in which the task is being created
- `[in] resource_name`: Identifier for the kind of resource that is being provided for diagnostic information exposed by the `async_hooks` API


### Factory

```cpp
static Napi::AsyncPromise AsyncPromise::New<T>(Napi::Env env, const char* resource_name, const Object& resource);
```

Constructs and initializes an instance of `AsyncPromise`
for a task of type `<T>`.

When this factory method is used, the task initialization handler `OnInit()`
with no arguments will be invoked.

#### Arguments
- `[in] env`: The environment in which the task is being created
- `[in] resource_name`: Identifier for the kind of resource that is being provided for diagnostic information exposed by the `async_hooks` API
- `[in] resource`: An object associated with the async work that will be passed to possible `async_hooks` init hooks


### Factory

```cpp
static Napi::AsyncPromise AsyncPromise::New<T>(const CallbackInfo& info);
```

Constructs and initializes an instance of `AsyncPromise`
for a task of type `<T>`.

When this factory method is used, the task initialization handler `OnInit()`
with a `CallbackInfo` argument will be invoked.

#### Arguments
- `[in] info`: The JavaScript context provided to the native callback


### Factory

```cpp
static Napi::AsyncPromise AsyncPromise::New<T>(const CallbackInfo& info, const char* resource_name);
```

Constructs and initializes an instance of `AsyncPromise`
for a task of type `<T>`.

When this factory method is used, the task initialization handler `OnInit()`
with a `CallbackInfo` argument will be invoked.

#### Arguments
- `[in] info`: The JavaScript context provided to the native callback
- `[in] resource_name`: Identifier for the kind of resource that is being provided for diagnostic information exposed by the `async_hooks` API


### Factory

```cpp
static Napi::AsyncPromise AsyncPromise::New<T>(const CallbackInfo& info, const char* resource_name, const Object& resource);
```

Constructs and initializes an instance of `AsyncPromise`
for a task of type `<T>`.

When this factory method is used, the task initialization handler `OnInit()`
with a `CallbackInfo` argument will be invoked.

#### Arguments
- `[in] info`: The JavaScript context provided to the native callback
- `[in] resource_name`: Identifier for the kind of resource that is being provided for diagnostic information exposed by the `async_hooks` API
- `[in] resource`: An object associated with the async work that will be passed to possible `async_hooks` init hooks


### Constructor

```cpp
Napi::AsyncPromise<T>::AsyncPromise(AsyncPromise&& other);
```

Constructs and initializes an instance of `AsyncPromise`
by taking ownership from another instance.

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


### Task

```cpp
<T> * Napi::AsyncPromise<T>::Task();
```

Requests a pointer to the instance of the user-defined task that will be scheduled.
Note that after the task has been queued, access to task instance may result in
undefined behavior.

#### Arguments
None


## AsyncPromise Operators

### napi_async_work

```cpp
Napi::AsyncPromise<T>::operator napi_async_work() const;
```

Returns the N-API `napi_async_work` wrapped by the task instance. This
can be used to mix usage of the C N-API and node-addon-api.


### napi_deferred

```cpp
Napi::AsyncPromise<T>::operator napi_deferred() const;
```

Returns the N-API `napi_deferred` wrapped by the task instance. This
can be used to mix usage of the C N-API and node-addon-api.


## Example

The basic usage pattern for `Napi::AsyncPromise` is summarized in these steps:

1. Declare a subclass of `Napi::AsyncPromise::BaseTask`
1. In the subclass, define an override of the appropriate `OnInit()` method
1. In the subclass, define an override of the `Execute()` abstract method
1. In the N-API callback function, create an instance of `Napi::AsyncPromise`
using the appropriate factory method and return its promise

As noted above, the the other virtual methods of `Napi::AsyncPromise::BaseTask`
may be overridden to provide custom behavior.

The code below demonstrates a basic example of using `Napi::AsyncPromise`:

```cpp
#include <napi.h>

#include <chrono>
#include <thread>

// Native routine registered with N-API as a JavaScript callback
Napi::Value PromisingEcho(const Napi::CallbackInfo & info) {
  // Define the task subclass
  class EchoTask : public Napi::AsyncPromise::BaseTask {
  private:
    std::string _echo_str;

    void OnInit(const Napi::CallbackInfo & info) override {
      if (info.Length() < 1 || !info[0].IsString()) {
        // Bad argument - reject the promise
        // This will fulfill the promise and skip the task
        Reject(Napi::TypeError::New(info.Env(), "Echo requires a string argument").Value());
      }
    }

    void Execute() override {
      // Simulate an long-running task running on a worker thread
      std::this_thread::sleep_for(std::chrono::seconds(1));

      // Set data for promise fulfillment
      _echo_str += " ... done";
    }

    void OnOK() override {
      // Echo the message to any awaiters
      Resolve(Napi::String::New(Env(), _echo_str));
    }
  }

  // Declare the task and return its promise
  return Napi::AsyncPromise::New<EchoTask>(info).Promise();
}
```
