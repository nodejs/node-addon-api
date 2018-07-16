# ArrayBuffer

The `ArrayBuffer` class corresponds to the
[JavaScript `ArrayBuffer`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer)
class.

## Methods

### New

Allocates a new `ArrayBuffer` instance with a given length.

```cpp
static ArrayBuffer New(napi_env env, size_t byteLength);
```

- `[in] env`: The environment in which to create the `ArrayBuffer` instance.
- `[in] byteLength`: The length to be allocated, in bytes.

Returns a new `ArrayBuffer` instance.

### New

Wraps the provided external data into a new `ArrayBuffer` instance.

The `ArrayBuffer` instance does not assume ownership for the data and expects it
to be valid for the lifetime of the instance. Since the `ArrayBuffer` is subject
to garbage collection this overload is only suitable for data which is static
and never needs to be freed.

```cpp
static ArrayBuffer New(napi_env env, void* externalData, size_t byteLength);
```

- `[in] env`: The environment in which to create the `ArrayBuffer` instance.
- `[in] externalData`: The pointer to the external data to wrap.
- `[in] byteLength`: The length of the `externalData`, in bytes.

Returns a new `ArrayBuffer` instance.

### New

Wraps the provided external data into a new `ArrayBuffer` instance.

The `ArrayBuffer` instance does not assume ownership for the data and expects it
to be valid for the lifetime of the instance. The data can only be freed once
the `finalizeCallback` is invoked to indicate that the `ArrayBuffer` has been
released.

```cpp
template <typename Finalizer>
static ArrayBuffer New(napi_env env,
                       void* externalData,
                       size_t byteLength,
                       Finalizer finalizeCallback);
```

- `[in] env`: The environment in which to create the `ArrayBuffer` instance.
- `[in] externalData`: The pointer to the external data to wrap.
- `[in] byteLength`: The length of the `externalData`, in bytes.
- `[in] finalizeCallback`: A function to be called when the `ArrayBuffer` is
  destroyed. It must implement `operator()`, accept a `void*` (which is the
  `externalData` pointer), and return `void`.

Returns a new `ArrayBuffer` instance.

### New

Wraps the provided external data into a new `ArrayBuffer` instance.

The `ArrayBuffer` instance does not assume ownership for the data and expects it
to be valid for the lifetime of the instance. The data can only be freed once
the `finalizeCallback` is invoked to indicate that the `ArrayBuffer` has been
released.

```cpp
template <typename Finalizer, typename Hint>
static ArrayBuffer New(napi_env env,
                       void* externalData,
                       size_t byteLength,
                       Finalizer finalizeCallback,
                       Hint* finalizeHint);
```

- `[in] env`: The environment in which to create the `ArrayBuffer` instance.
- `[in] externalData`: The pointer to the external data to wrap.
- `[in] byteLength`: The length of the `externalData`, in bytes.
- `[in] finalizeCallback`: The function to be called when the `ArrayBuffer` is
  destroyed. It must implement `operator()`, accept a `void*` (which is the
  `externalData` pointer) and `Hint*`, and return `void`.
- `[in] finalizeHint`: The hint to be passed as the second parameter of the
  finalize callback.

Returns a new `ArrayBuffer` instance.

### Constructor

Initializes an empty instance of the `ArrayBuffer` class.

```cpp
ArrayBuffer();
```

### Constructor

Initializes a wrapper instance of an existing `ArrayBuffer` object.

```cpp
ArrayBuffer(napi_env env, napi_value value);
```

- `[in] env`: The environment in which to create the `ArrayBuffer` instance.
- `[in] value`: The `ArrayBuffer` reference to wrap.

### ByteLength

```cpp
size_t ByteLength() const;
```

Returns the length of the wrapped data, in bytes.

### Data

```cpp
T* Data() const;
```

Returns a pointer the wrapped data.
