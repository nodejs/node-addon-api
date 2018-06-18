You are reading a draft of the next documentation and it's in continuous update so
if you don't find what you need please refer to:
[C++ wrapper classes for the ABI-stable C APIs for Node.js](https://nodejs.github.io/node-addon-api/)

# Promise

The Promise class, along with its Promise::Deferred class, implement the ability to create, resolve, and reject Promise objects.

The basic approach is to create a Promise::Deferred object and return to your caller the value returned by the Promise::Deferred::Promise method. For example:

```cpp
Value YourFunction(const CallbackInfo& info) {
  // your code goes here...
  Promise::Deferred deferred = Promise::Deferred::New(info.Env());
  // deferred needs to survive this call...
  return deferred.Promise();
}
```

Later, when the asynchronous process completes, call either the `Resolve` or `Reject` method on the Promise::Deferred object created earlier:

```cpp
  deferred.Resolve(String::New(info.Env(), "OK"));
```

## Promise::Deferred Methods

### Factory Method

```cpp
static Promise::Deferred Promise::Deferred::New(napi_env env);
```

* `[in] env`: The `napi_env` environment in which to create the Deferred object.

### Constructor

```cpp
Promise::Deferred(napi_env env);
```

* `[in] env`: The `napi_env` environment in which to construct the Deferred object.

### Promise

```cpp
Promise Promise::Deferred::Promise() const;
```

Returns the Promise object held by the Promise::Deferred object.

### Resolve

```cpp
void Promise::Deferred::Resolve(napi_value value) const;
```

Resolves the Promise object held by the Promise::Deferred object.

* `[in] value`: The N-API primitive value with which to resolve the Promise.

### Reject

```cpp
void Promise::Deferred::Reject(napi_value value) const;
```

Rejects the Promise object held by the Promise::Deferred object.

* `[in] value`: The N-API primitive value with which to reject the Promise.
