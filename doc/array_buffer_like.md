# ArrayBufferLike

Class `Napi::ArrayBufferLike` inherits from class [`Napi::Object`][].

The `Napi::ArrayBufferLike` class is the base class for [`Napi::ArrayBuffer`][]
and [`Napi::SharedArrayBuffer`][]. It adds common array buffer capabilities to
both. It is an abstract-only base class.

### ByteLength

```cpp
size_t Napi::ArrayBufferLike::ByteLength() const;
```

Returns the length of the wrapped data, in bytes.

### Data

```cpp
void* Napi::ArrayBufferLike::Data() const;
```

Returns a pointer the wrapped data.

[`Napi::ArrayBuffer`]: ./array_buffer.md
[`Napi::Object`]: ./object.md
[`Napi::SharedArrayBuffer`]: ./shared_array_buffer.md
