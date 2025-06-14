# Promise

Class `Napi::Promise` inherits from class [`Napi::Object`][].

The `Napi::Promise` class, along with its `Napi::Promise::Deferred` class, implement the ability to create, resolve, and reject Promise objects.

The basic approach is to create a `Napi::Promise::Deferred` object and return to your caller the value returned by the `Napi::Promise::Deferred::Promise` method. For example:

```cpp
Napi::Value YourFunction(const Napi::CallbackInfo& info) {
  // your code goes here...
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(info.Env());
  // deferred needs to survive this call...
  return deferred.Promise();
}
```

Later, when the asynchronous process completes, call either the `Resolve` or `Reject` method on the `Napi::Promise::Deferred` object created earlier:

```cpp
  deferred.Resolve(String::New(info.Env(), "OK"));
```

## Promise::Deferred Methods

### Factory Method

```cpp
static Napi::Promise::Deferred Napi::Promise::Deferred::New(napi_env env);
```

* `[in] env`: The `napi_env` environment in which to create the `Napi::Promise::Deferred` object.

### Constructor

```cpp
Napi::Promise::Deferred(napi_env env);
```

* `[in] env`: The `napi_env` environment in which to construct the `Napi::Promise::Deferred` object.

### Env

```cpp
Napi::Env Napi::Promise::Deferred::Env() const;
```

Returns the Env environment this `Napi::Promise::Deferred` object is associated with.

### Promise

```cpp
Napi::Promise Napi::Promise::Deferred::Promise() const;
```

Returns the `Napi::Promise` object held by the `Napi::Promise::Deferred` object.

### Resolve

```cpp
void Napi::Promise::Deferred::Resolve(napi_value value) const;
```

Resolves the `Napi::Promise` object held by the `Napi::Promise::Deferred` object.

* `[in] value`: The Node-API primitive value with which to resolve the `Napi::Promise`.

### Reject

```cpp
void Napi::Promise::Deferred::Reject(napi_value value) const;
```

Rejects the Promise object held by the `Napi::Promise::Deferred` object.

* `[in] value`: The Node-API primitive value with which to reject the `Napi::Promise`.

## Promise Methods

### Then

```cpp
Napi::Promise Napi::Promise::Then(napi_value onFulfilled) const;
Napi::Promise Napi::Promise::Then(const Function& onFulfilled) const;
```

Attaches a fulfillment handler to the promise and returns a new promise.

**Parameters:**
* `[in] onFulfilled`: The fulfillment handler for the promise. May be any of:
  - `napi_value` – a JavaScript function to be called when the promise is fulfilled.
  - `const Function&` – the [`Napi::Function`](function.md) to be called when the promise is fulfilled.

**Returns:** A new `Napi::Promise` that resolves or rejects based on the handler's result.

### Then

```cpp
Napi::Promise Napi::Promise::Then(napi_value onFulfilled, napi_value onRejected) const;
Napi::Promise Napi::Promise::Then(const Function& onFulfilled,
                                  const Function& onRejected) const;
```

Attaches a fulfillment and rejection handlers to the promise and returns a new promise.

**Parameters:**
* `[in] onFulfilled`: The fulfillment handler for the promise. May be any of:
  - `napi_value` – a JavaScript function to be called when the promise is fulfilled.
  - `const Function&` – the [`Napi::Function`](function.md) to be called when the promise is fulfilled.
* `[in] onRejected` (optional): The rejection handler for the promise. May be any of:
  - `napi_value` – a JavaScript function to be called when the promise is rejected.
  - `const Function&` – the [`Napi::Function`](function.md) to be called when the promise is rejected.

### Catch
```cpp
Napi::Promise Napi::Promise::Catch(napi_value onRejected) const;
Napi::Promise Napi::Promise::Catch(const Function& onRejected) const;
```

Attaches a rejection handler to the promise and returns a new promise.

**Parameters:**
* `[in] onRejected`: The rejection handler for the promise. May be any of:
  - `napi_value` – a JavaScript function to be called when the promise is rejected.
  - `const Function&` – the [`Napi::Function`](function.md) to be called when the promise is rejected.

**Returns:** A new `Napi::Promise` that handles rejection cases.

[`Napi::Object`]: ./object.md
[`Napi::Function`]: ./function.md
