# Finalization

Various node-addon-api methods accept a templated `Finalizer finalizeCallback`
parameter. This parameter represents a native callback function that runs in
response to a garbage collection event. A finalizer is considered a _basic_
finalizer if the callback only utilizes a certain subset of APIs, which may
provide more efficient memory management, optimizations, improved execution, or
other benefits.

In general, it is best to use basic finalizers whenever possible (eg. when
access to JavaScript is _not_ needed). The
`NODE_ADDON_API_REQUIRE_BASIC_FINALIZERS` preprocessor directive can be defined
to ensure that all finalizers are basic.

## Finalizers

The callback takes `Napi::Env` as its first argument:

### Example

```cpp
Napi::External<int>::New(Env(), new int(1), [](Napi::Env env, int* data) {
  env.RunScript("console.log('Finalizer called')");
  delete data;
});
```

## Basic Finalizers

Use of basic finalizers may allow the engine to perform optimizations when
scheduling or executing the callback. For example, V8 does not allow access to
the engine heap during garbage collection. Restricting finalizers from accessing
the engine heap allows the callback to execute during garbage collection,
providing a chance to free native memory eagerly.

In general, APIs that access engine heap are not allowed in basic finalizers.

The callback takes `Napi::BasicEnv` as its first argument:

### Example

```cpp
Napi::ArrayBuffer::New(
    Env(), data, length, [](Napi::BasicEnv /*env*/, void* finalizeData) {
      delete[] static_cast<uint8_t*>(finalizeData);
    });
```

## Scheduling Finalizers

In addition to passing finalizers to `Napi::External`s and other Node-API
constructs, `Napi::BasicEnv::PostFinalize(Napi::BasicEnv, Finalizer)` can be
used to schedule a callback to run outside of the garbage collector
finalization. Since the associated native memory may already be freed by the
basic finalizer, any additional data may be passed eg. via the finalizer's
parameters (`T data*`, `Hint hint*`) or via lambda capture. This allows for
freeing native data in a basic finalizer, while executing any JavaScript code in
an additional finalizer.

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

// Basic finalizer to free `LargeData`. Takes ownership of the pointer and
// frees its memory after use.
void MyBasicFinalizer(Napi::BasicEnv env, LargeData* data) {
  std::unique_ptr<LargeData> instance(data);
  std::cout << "Basic finalizer for instance " << instance->id
            << " called\n";

  // Register a finalizer. Since the instance will be deleted by
  // the time this callback executes, pass the instance's `id` via lambda copy
  // capture and _not_ a reference capture that accesses `this`.
  env.PostFinalizer([instanceId = instance->id](Napi::Env env) {
    env.RunScript("console.log('Finalizer for instance " +
                  std::to_string(instanceId) + " called');");
  });

  // Free the `LargeData` held in `data` once `instance` goes out of scope.
}

Value CreateExternal(const CallbackInfo& info) {
  // Create a new instance of LargeData.
  auto instance = std::make_unique<LargeData>();

  // Wrap the instance in an External object, registering a basic
  // finalizer that will delete the instance to free the "large" amount of
  // memory.
  return External<LargeData>::New(info.Env(), instance.release(), MyBasicFinalizer);
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
  // ... do something with `ext` ..
}

console.log('Loop complete');
await new Promise(resolve => setImmediate(resolve));
console.log('Next event loop cycle');
```

Possible output:

```
Basic finalizer for instance 0 called
Basic finalizer for instance 1 called
Basic finalizer for instance 2 called
Basic finalizer for instance 3 called
Basic finalizer for instance 4 called
Loop complete
Finalizer for instance 3 called
Finalizer for instance 4 called
Finalizer for instance 1 called
Finalizer for instance 2 called
Finalizer for instance 0 called
Next event loop cycle
```

If the garbage collector runs during the loop, the basic finalizers execute and
display their logging message synchronously during the loop execution. The
additional finalizers execute at some later point after the garbage collection
cycle.
