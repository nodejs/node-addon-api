# SharedArrayBuffer

Class `Napi::SharedArrayBuffer` inherits from class [`Napi::Object`][].

The `Napi::SharedArrayBuffer` class corresponds to the
[JavaScript `SharedArrayBuffer`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer)
class.

**NOTE**: The support for `Napi::SharedArrayBuffer` is only available when using
`NAPI_EXPERIMENTAL` and building against Node.js headers that support this
feature.

## Methods

### New

Allocates a new `Napi::SharedArrayBuffer` instance with a given length.

```cpp
static Napi::SharedArrayBuffer Napi::SharedArrayBuffer::New(napi_env env, size_t byteLength);
```

- `[in] env`: The environment in which to create the `Napi::SharedArrayBuffer`
  instance.
- `[in] byteLength`: The length to be allocated, in bytes.

Returns a new `Napi::SharedArrayBuffer` instance.

### Constructor

Initializes an empty instance of the `Napi::SharedArrayBuffer` class.

```cpp
Napi::SharedArrayBuffer::SharedArrayBuffer();
```

### Constructor

Initializes a wrapper instance of an existing `Napi::SharedArrayBuffer` object.

```cpp
Napi::SharedArrayBuffer::SharedArrayBuffer(napi_env env, napi_value value);
```

- `[in] env`: The environment in which to create the `Napi::SharedArrayBuffer`
  instance.
- `[in] value`: The `Napi::SharedArrayBuffer` reference to wrap.

### ByteLength

```cpp
size_t Napi::SharedArrayBuffer::ByteLength() const;
```

Returns the length of the wrapped data, in bytes.

### Data

```cpp
void* Napi::SharedArrayBuffer::Data() const;
```

Returns a pointer the wrapped data.

[`Napi::Object`]: ./object.md
