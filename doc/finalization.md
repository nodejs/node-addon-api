# Finalization

Various node-addon-api methods accept a templated `Finalizer finalizeCallback`
parameter. This parameter represents a callback function that runs in a
_synchronous_ or _asynchronous_ manner in response to a garbage collection
event. The callback executes either during the garbage collection cycle
(_synchronously_) or after the cycle has completed (_asynchronously_).

In general, it is best to use synchronous finalizers whenever a callback must
free native data.

## Synchronous Finalizers

Synchronous finalizers execute during the garbage collection cycle, and
therefore must not manipulate the engine heap. The finalizer executes in the
current tick, providing a chance to free native memory. The callback takes
`Napi::BasicEnv` as its first argument.

### Example

```cpp
Napi::ArrayBuffer::New(
    Env(), data, length, [](Napi::BasicEnv /*env*/, void* finalizeData) {
      delete[] static_cast<uint8_t*>(finalizeData);
    });
```

## Asynchronous Finalizers

Asynchronous finalizers execute outside the context of garbage collection later
in the event loop. Care must be taken when creating several objects in
succession if the finalizer must free native memory. The engine may garbage
collect several objects created in one tick, but any asynchronous finalizers --
and therefore the freeing of native memory -- will not execute in the same tick.
The callback takes `Napi::Env` as its first argument.

### Example

```cpp
Napi::External<int>::New(Env(), new int(1), [](Napi::Env env, int* data) {
  env.RunScript("console.log('Finalizer called')");
  delete data;
});
```

#### Caveats

```js
while (conditional()) {
  const external = createExternal();
  doSomethingWith(external);
}
```

The engine may determine to run the garbage collector within the loop, freeing
the JavaScript engine's resources for all objects created by `createExternal()`.
However, asynchronous finalizers will not execute during this loop, and the
native memory held in `data` will not be freed.

## Scheduling Asynchronous Finalizers

In addition to passing asynchronous finalizers to externals and other Node-API
constructs, use `Napi::BasicEnv::PostFinalize(Napi::Env, Finalizer)` to schedule
an asynchronous finalizer to run after the next garbage collection cycle
completes.

Free native data in a synchronous finalizer, while executing any JavaScript code
in an asynchronous finalizer attached via this method. Since the associated
native memory may already be freed by the synchronous finalizer, any additional
data may be passed eg. via the finalizer's parameters (`T data*`, `Hint hint*`)
or via lambda capture.

### Example

```cpp
// Native Add-on

#include <iostream>
#include <memory>
#include "napi.h"

using namespace Napi;

// A structure representing some data that uses a "large" amount of memory.
class LargeData {
 public:
  LargeData() : id(instances++) {}
  size_t id;

  static size_t instances;
};

size_t LargeData::instances = 0;

// Synchronous finalizer to free `LargeData`. Takes ownership of the pointer and
// frees its memory after use.
void MySyncFinalizer(Napi::BasicEnv env, LargeData* data) {
  std::unique_ptr<LargeData> instance(data);
  std::cout << "Synchronous finalizer for instance " << instance->id
            << " called\n";

  // Register the asynchronous callback. Since the instance will be deleted by
  // the time this callback executes, pass the instance's `id` via lambda copy
  // capture and _not_ a reference capture that accesses `this`.
  env.PostFinalizer([instanceId = instance->id](Napi::Env env) {
    std::cout << "Asynchronous finalizer for instance " << instanceId
              << " called\n";
  });

  // Free the `LargeData` held in `data` once `instance` goes out of scope.
}

Value CreateExternal(const CallbackInfo& info) {
  // Create a new instance of LargeData.
  auto instance = std::make_unique<LargeData>();

  // Wrap the instance in an External object, registering a synchronous
  // finalizer that will delete the instance to free the "large" amount of
  // memory.
  auto ext =
      External<LargeData>::New(info.Env(), instance.release(), MySyncFinalizer);

  return ext;
}

Object Init(Napi::Env env, Object exports) {
  exports["createExternal"] = Function::New(env, CreateExternal);
  return exports;
}

NODE_API_MODULE(addon, Init)
```

```js
// JavaScript

const { createExternal } = require('./addon.node');

for (let i = 0; i < 5; i++) {
  const ext = createExternal();
  doSomethingWith(ext);
}

console.log('Loop complete');
await new Promise(resolve => setImmediate(resolve));
console.log('Next event loop cycle');

```

Possible output:

```
Synchronous finalizer for instance 0 called
Synchronous finalizer for instance 1 called
Synchronous finalizer for instance 2 called
Synchronous finalizer for instance 3 called
Synchronous finalizer for instance 4 called
Loop complete
Asynchronous finalizer for instance 3 called
Asynchronous finalizer for instance 4 called
Asynchronous finalizer for instance 1 called
Asynchronous finalizer for instance 2 called
Asynchronous finalizer for instance 0 called
Next event loop cycle
```

If the garbage collector runs during the loop, the synchronous finalizers
execute and display their logging message before the loop completes. The
asynchronous finalizers execute at some later point after the garbage collection
cycle.