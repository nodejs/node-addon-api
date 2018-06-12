# AsyncWorker

`AsyncWorker` is an abstract class that you can subclass to remove many of the
tedious tasks of moving data between the event loop and worker threads. This
class internally handles all the details of creating and executing an asynchronous
operation.

## Methods

### Env

Requests the environment in which the async worker has been initially created.

```cpp
Env Env() const;
```

Returns the environment in which the async worker has been created.

### Queue

Requests that the created work or task will be queued for execution.

```cpp
void Queue();
```

### Cancel

Cancels queued work if it has not yet been started. If it has already started
executing, it cannot be cancelled.

```cpp
void Cancel();
```

### Receiver

```cpp
ObjectReference& Receiver();
```

Returns the persistent object reference of the receiver object set when the async
worker was created.

### Callback

```cpp
FunctionReference& Callback();
```

Returns the persistent function reference of the callback set when the async
worker was created. The returned function reference will receive the results of
the computation that happened in the `Execute` method.

### SetError

Sets the error message for the error that happened during the execution.

```cpp
void SetError(const std::string& error);
```

- `[in] error`: The reference to the string that represent the message of the error.

### Execute

This method is used to execute some tasks out of the **event loop** on a libuv
worker thread.

```cpp
virtual void Execute() = 0;
```

### OnOK

This method represents a callback that is invoked when the computation in the
`Excecute` method ends.

```cpp
virtual void OnOK();
```

### OnError

```cpp
virtual void OnError(const Error& e);
```

### Constructor

Creates a new `AsyncWorker`.

```cpp
explicit AsyncWorker(const Function& callback);
```

- ```[in] callback```: The function which will be called when an asynchronous
operations ends. The given function is called from the main event loop thread.

### Constructor

Creates a new `AsyncWorker`.

```cpp
explicit AsyncWorker(const Function& callback, const char* resource_name);
```

- ```[in] callback```: The function which will be called when an asynchronous
operations ends. The given function is called from the main event loop thread.
- ```[in] resource_name```: Null-terminated strings that represents the
identifier for the kind of resource that is being provided for diagnostic
information exposed by the async_hooks API.

### Constructor

Creates a new `AsyncWorker`.

```cpp
explicit AsyncWorker(const Function& callback, const char* resource_name, const Object& resource);
```

- ```[in] callback```: The function which will be called when an asynchronous
operations ends. The given function is called from the main event loop thread.
- ```[in] resource_name```:  Null-terminated strings that represents the
identifier for the kind of resource that is being provided for diagnostic
information exposed by the async_hooks API.
- ```[in] resource```: Object associated with the asynchronous operation that
will be passed to possible async_hooks.

### Constructor

Creates a new `AsyncWorker`.

```cpp
explicit AsyncWorker(const Object& receiver, const Function& callback);
```

- ```[in] receiver```: The `this` object passed to the called function.
- ```[in] callback```: The function which will be called when an asynchronous
operations ends. The given function is called from the main event loop thread.

### Constructor

Creates a new `AsyncWorker`.

```cpp
explicit AsyncWorker(const Object& receiver, const Function& callback,const char* resource_name);
```

- ```[in] receiver```: The `this` object passed to the called function.
- ```[in] callback```: The function which will be called when an asynchronous
operations ends. The given function is called from the main event loop thread.
- ```[in] resource_name```:  Null-terminated strings that represents the
identifier for the kind of resource that is being provided for diagnostic
information exposed by the async_hooks API.

### Constructor

Creates a new `AsyncWorker`.

```cpp
explicit AsyncWorker(const Object& receiver, const Function& callback, const char* resource_name, const Object& resource);
```

- ```[in] receiver```: The `this` object passed to the called function.
- ```[in] callback```: The function which will be called when an asynchronous
operations ends. The given function is called from the main event loop thread.
- ```[in] resource_name```:  Null-terminated strings that represents the
identifier for the kind of resource that is being provided for diagnostic
information exposed by the async_hooks API.
- ```[in] resource```: Object associated with the asynchronous operation that
will be passed to possible async_hooks.

### Destructor

Deletes the created work object that is used to execute logic asynchronously.

```cpp
virtual ~AsyncWorker();
```

## Operator

```cpp
operator napi_async_work() const;
```

Returns the N-API napi_async_work wrapped by the AsyncWorker object. This can be
used to mix usage of the C N-API and node-addon-api.

## Example

The first step to use the `AsyncWorker` class is to create a new class that inherit
from it and implement the `Execute` abstract method. Typically input to your
worker will be saved within class' fields generally passed in through its
constructor.

When the `Execute` method completes without errors the `OnOK` function callback
will be invoked. In this function the results of the computation will be
reassembled and returned back to the initial JavaScript context.

`AsyncWorker` ensure that all the code in the `Execute` function runs in the
background out of the **event loop** thread and at the end the `OnOK` or `OnError`
function will be called and are executed as part of the event loop.

The code below show a basic example of `AsyncWorker` the implementation:

```cpp
#include<napi.h>

#include <chrono>
#include <thread>

use namespace Napi;

class EchoWorker : public AsyncWorker {
    public:
        EchoWorker(Function& callback, std::string& echo)
        : AsyncWorker(callback), echo(echo) {}

        ~EchoWorker() {}
    // This code will be executed on the worker thread
    void Execute() {
        // Need to simulate cpu heavy task
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    void OnOK() {
        HandleScope scope(Env());
        Callback().Call({Env().Null(), String::New(Env(), echo)});
    }

    private:
        std::string echo;
};
```

The `EchoWorker`'s contructor call the base class' constructor to pass in the
callback that the `AsyncWorker` base class will store persistently. When the work
on the `Execute` method is done the `OnOk` method is called and the results return
back to JavaScript invoking the stored callback with its associated environment.

The following code shows an example on how to create and and use an `AsyncWorker`

```cpp
Value Echo(const CallbackInfo& info) {
    // You need to check the input data here
    Function cb = info[1].As<Function>();
    std::string in = info[0].As<String>();
    EchoWorker* wk = new EchoWorker(cb, in);
    wk->Queue();
    return info.Env().Undefined();
```

Using the implementation of an `AsyncWorker` is very simple. You need only create
a new instance and pass to its constructor the callback you want to execute when
your asynchronous task ends and other data you need for your computation. The
only action you have to do is to call the `Queue` method that will that will
queue the created worker for execution.