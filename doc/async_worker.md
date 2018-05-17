# AsyncWorker

`AsyncWorker` is an abstract class that you can subclass to remove much of the
tedious tasks on moving data between the event loop and worker threads. This
class internally handles all the details of creating and executing asynchronous
operation.

## Methods

### Env

Requests the environment in which the async worker has been initially created.

```cpp
Env Env() const;
```

Returns the environment in which the async worker has been created.

### Queue

Requests that the created work or task will be placed on the queue of the execution.

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

Returns the persistent object reference of the receiver objet set when the async
worker is created.

### Callback

```cpp
FunctionReference& Callback();
```

Returns the persistent function reference of the callback set when the async
worker is created. The returned function reference will be called passing it the
results of the computation happened on the `Execute` method.

### SetError

Sets the error message for the error happened during the execution.

```cpp
void SetError(const std::string& error);
```

- `[in] error`: The reference to string that represent the message of the error.


### Execute

This method is used to execute some tasks out of the **event loop** on a libuv 
worker thread. 

```cpp
virtual void Execute() = 0;
```

### OnOK

This method represents a callback that is invoked when the computation on the
`Excecute` method end.

```cpp
virtual void OnOK();
```

### OnError

```cpp
virtual void OnError(const Error& e);
```

### Constructor

Creates new async worker

```cpp
explicit AsyncWorker(const Function& callback);
```

### Constructor

Creates new async worker

```cpp
explicit AsyncWorker(const Object& receiver, const Function& callback);
```

### Destructor

Deletes the created work object that is used to execute logic asynchronously

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

The first step to use `AsyncWorker` class is to create a new class that inherit
from it and implement the `Execute` abstract method. Typically input to your
worker will be saved within your fields' class generally passed in through its
constructor.

When the `Execute` method complete without errors the `OnOK` function callback
will be invoked. In this function the results of the computation will be
reassembled and returned back to initial JavaScript context.

`AsyncWorker` ensure that all the code inside in the `Execute` function runs in
background out of the **event loop** thread and at the end `OnOK` or `OnError` 
function will be called and are executed as part of the event loop.

The code below show a basic example of `AsyncWorker` implementation:

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

The `EchoWorker`'s contructor call the base class's constructor to pass in the
callback that the `AsyncWorker` base class will store persistently. When the work
on the `Execute` method is done the `OnOk` method is called and the results return
back to JavaScript invoking the stored callback with its associated environment.

The following code show an example on how to create and and use an `AsyncWorker`

```cpp
Value Echo(const CallbackInfo& info) {
    // You need to check the input data here
    Function cb = info[1].As<Function>();
    std::string in = info[0].As<String>();
    EchoWorker* wk = new EchoWorker(cb, in);
    wk->Queue();
    return info.Env().Undefined();
```

Use the implementation of an `AsyncWorker` is very simple you need only to
create a new instance and pass to its constructor the callback you want exectute
when your asynchronous task end and other data you need for your computation. The
only action you have to do is to call the `Queue` method that will place the
crearted worker on the queue of the execution.