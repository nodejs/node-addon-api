# TypedArray

The `TypedArray` class corresponds to the
[JavaScript `TypedArray`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/TypedArray)
class.

## Methods

### Constructor

Initializes an empty instance of the `TypedArray` class.

```cpp
TypedArray();
```

### Constructor

Initializes a wrapper instance of an existing `TypedArray` instance.

```cpp
TypedArray(napi_env env, napi_value value);
```

- `[in] env`: The environment in which to create the `TypedArray` instance.
- `[in] value`: The `TypedArray` reference to wrap.

### TypedArrayType

```cpp
napi_typedarray_type TypedArrayType() const;
```

Returns the type of this instance.

### ArrayBuffer

```cpp
Napi::ArrayBuffer ArrayBuffer() const;
```

Returns the backing array buffer.

### ElementSize

```cpp
uint8_t ElementSize() const;
```

Returns the size of one element, in bytes.

### ElementLength

```cpp
size_t ElementLength() const;
```

Returns the number of elements.

### ByteOffset

```cpp
size_t ByteOffset() const;
```

Returns the offset into the `ArrayBuffer` where the array starts, in bytes.

### ByteLength

```cpp
size_t ByteLength() const;
```

Returns the length of the array, in bytes.
