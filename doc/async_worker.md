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

### Callback

```cpp
FunctionReference& Callback();
```

### SetError

```cpp
void SetError(const std::string& error);
```

### Execute

```cpp
virtual void Execute() = 0;
```

### OnOK

```cpp
virtual void OnOK();
```

### OnError

```cpp
virtual void OnError(const Error& e);
```