# Buffer

The `Buffer` class creates a projection of raw data that can be consumed by
script.

## Methods

### New

Allocates a new `Buffer` object with a given length.

```cpp
static Buffer<T> New(napi_env env, size_t length);
```

- `[in] env`: The environment in which to create the `Buffer` object.
- `[in] length`: The number of `T` elements to allocate.

Returns a new `Buffer` object.

### New

Wraps the provided external data into a new `Buffer` object.

The `Buffer` object does not assume ownership for the data and expects it to be
valid for the lifetime of the object. Since the `Buffer` is subject to garbage
collection this overload is only suitable for data which is static and never
needs to be freed.

```cpp
static Buffer<T> New(napi_env env, T* data, size_t length);
```

- `[in] env`: The environment in which to create the `Buffer` object.
- `[in] data`: The pointer to the external data to expose.
- `[in] length`: The number of `T` elements in the external data.

Returns a new `Buffer` object.

### New

Wraps the provided external data into a new `Buffer` object.

The `Buffer` object does not assume ownership for the data and expects it
to be valid for the lifetime of the object. The data can only be freed once the
`finalizeCallback` is invoked to indicate that the `Buffer` has been released.

```cpp
template <typename Finalizer>
static Buffer<T> New(napi_env env,
                     T* data,
                     size_t length,
                     Finalizer finalizeCallback);
```

- `[in] env`: The environment in which to create the `Buffer` object.
- `[in] data`: The pointer to the external data to expose.
- `[in] length`: The number of `T` elements in the external data.
- `[in] finalizeCallback`: The function to be called when the `Buffer` is
  destroyed. It must implement `operator()`, accept a `T*` (which is the
  external data pointer), and return `void`.

Returns a new `Buffer` object.

### New

Wraps the provided external data into a new `Buffer` object.

The `Buffer` object does not assume ownership for the data and expects it to be
valid for the lifetime of the object. The data can only be freed once the
`finalizeCallback` is invoked to indicate that the `Buffer` has been released.

```cpp
template <typename Finalizer, typename Hint>
static Buffer<T> New(napi_env env,
                     T* data,
                     size_t length,
                     Finalizer finalizeCallback,
                     Hint* finalizeHint);
```

- `[in] env`: The environment in which to create the `Buffer` object.
- `[in] data`: The pointer to the external data to expose.
- `[in] length`: The number of `T` elements in the external data.
- `[in] finalizeCallback`: The function to be called when the `Buffer` is
  destroyed. It must implement `operator()`, accept a `T*` (which is the
  external data pointer) and `Hint*`, and return `void`.
- `[in] finalizeHint`: The hint to be passed as the second parameter of the
  finalize callback.

Returns a new `Buffer` object.

### Copy

Allocates a new `Buffer` object and copies the provided external data into it.

```cpp
static Buffer<T> Copy(napi_env env, const T* data, size_t length);
```

- `[in] env`: The environment in which to create the `Buffer` object.
- `[in] data`: The pointer to the external data to copy.
- `[in] length`: The number of `T` elements in the external data.

Returns a new `Buffer` object containing a copy of the data.

### Constructor

Initializes an empty instance of the `Buffer` class.

```cpp
Buffer();
```

### Constructor

Initializes the `Buffer` object using an existing Uint8Array.

```cpp
Buffer(napi_env env, napi_value value);
```

- `[in] env`: The environment in which to create the `Buffer` object.
- `[in] value`: The Uint8Array reference to wrap.

### Data

```cpp
T* Data() const;
```

Returns a pointer the external data.

### Length

```cpp
size_t Length() const;
```

Returns the number of `T` elements in the external data.
