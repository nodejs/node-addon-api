**WORK IN PROGRESS, NOT YET COMPLETE**

# CallbackInfo

The object representing the components of the JavaScript request being made.

The CallbackInfo object is usually created and passed by the Node.js runtime or node-addon-api infrastructure.

The CallbackInfo object contains the arguments passed by the caller. The number of arguments is returned by the `Length` method. Each individual argument can be accessed using the `operator[]` method.

The `SetData` and `Data` methods are used to set and retrieve the data pointer contained in the CallbackInfo object.

## Methods

### Constructor

```cpp
CallbackInfo(napi_env env, napi_callback_info info);
```

- `[in] env`: The `napi_env` environment in which to construct the `CallbackInfo` object.
- `[in] info`: The `napi_callback_info` data structure from which to construct the `CallbackInfo` object.

### Env

```cpp
Napi::Env Env() const;
```

Returns the `Env` object in which the request is being made.

### NewTarget

```cpp
Value NewTarget() const;
```

Returns the `new.target` value of the constructor call. If the function that was invoked (and for which the CallbackInfo was passed) is not a constructor call, a call to `IsEmpty()` on the returned value returns true.

### IsConstructCall

```cpp
bool IsConstructCall() const;
```

Returns a `bool` indicating if the function that was invoked (and for which the CallbackInfo was passed) is a constructor call.


### Length

```cpp
size_t Length() const;
```

Returns the number of arguments passed in the CallbackInfo object.

### operator []

```cpp
const Value operator [](size_t index) const;
```

- `[in] index`: The zero-based index of the requested argument.

Returns a `Value` object containing the requested argument.

### This

```cpp
Value This() const;
```

Returns the JavaScript `this` value for the call

### Data

```cpp
void* Data() const;
```

Returns the data pointer for the callback.

### SetData

```cpp
void SetData(void* data);
```

- `[in] data`: The new data pointer to associate with this CallbackInfo object.

Returns `void`.

### Not documented here

```cpp
~CallbackInfo();
// Disallow copying to prevent multiple free of _dynamicArgs
CallbackInfo(CallbackInfo const &) = delete;
void operator=(CallbackInfo const &) = delete;
```
