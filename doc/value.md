**WORK IN PROGRESS, NOT YET COMPLETE**

# Value

Value is the C++ manifestation of a JavaScript value.

Value is a the base class upon which other JavaScript values such as Number, Boolean, String, and Object are based.

The following classes inherit, either directly or indirectly, from Value:

- [Array](array.md)
- [ArrayBuffer](array_buffer.md)
- [Boolean](boolean.md)
- [Buffer](buffer.md)
- [External](external.md)
- [Function](function.md)
- [Name](name.md)
- [Number](number.md)
- [Object](object.md)
- [String](string.md)
- [Symbol](symbol.md)
- [TypedArray](typed_array.md)
- [TypedArrayOf](typed_array_of.md)

## Methods

### Empty Constructor

```cpp
Value();
```

Creates a new *empty* Value instance.

### Constructor

```cpp
Value(napi_env env, napi_value value);
```

- `[in] env`: The `napi_env` environment in which to construct the Value object.

- `[in] value`: The C++ primitive from which to instantiate the Value. `value` may be any of:
  - bool
  - Any integer type
  - Any floating point type
  - const char* (encoded using UTF-8, null-terminated)
  - const char16_t* (encoded using UTF-16-LE, null-terminated)
  - std::string (encoded using UTF-8)
  - std::u16string
  - napi::Value
  - napi_value

### From

```cpp
template <typename T> static Value From(napi_env env, const T& value);
```

- `[in] env`: The `napi_env` environment in which to create the Value object.

- `[in] value`: The N-API primitive value from which to create the Value object.

Returns a Value object from an N-API primitive value.

### operator napi_value

```cpp
operator napi_value() const;
```

Returns this Value's N-API value primitive.

Returns `nullptr` if this Value is *empty*.

### operator ==

```cpp
bool operator ==(const Value& other) const;
```

- `[in] other`: The Value object to be compared.

Returns a `bool` indicating if this Value strictly equals another Value.

### operator !=

```cpp
bool operator !=(const Value& other) const;
```

- `[in] other`: The Value object to be compared.

Returns a `bool` indicating if this Value does not strictly equal another Value.

### StrictEquals

```cpp
bool StrictEquals(const Value& other) const;
```
- `[in] other`: The Value object to be compared.

Returns a `bool` indicating if this Value strictly equals another Value.

### Env

```cpp
Napi::Env Env() const;
```

Returns the `Env` environment this value is associated with.

### IsEmpty

```cpp
bool IsEmpty() const;
```

Returns a `bool` indicating if this Value is *empty* (uninitialized).

An empty Value is invalid, and most attempts to perform an operation on an empty Value will result in an exception. Note an empty Value is distinct from JavaScript `null` or `undefined`, which are valid values.

When C++ exceptions are disabled at compile time, a method with a `Value` return type may return an empty Value to indicate a pending exception. So when not using C++ exceptions, callers should check whether this Value is empty before attempting to use it.

### Type

```cpp
napi_valuetype Type() const;
```

Returns the `napi_valuetype` type of the Value.

### IsUndefined

```cpp
bool IsUndefined() const;
```

Returns a `bool` indicating if this Value is an undefined JavaScript value.

### IsNull

```cpp
bool IsNull() const;
```

Returns a `bool` indicating if this Value is a null JavaScript value.

### IsBoolean

```cpp
bool IsBoolean() const;
```

Returns a `bool` indicating if this Value is a JavaScript boolean.

### IsNumber

```cpp
bool IsNumber() const;
```

Returns a `bool` indicating if this Value is a JavaScript number.

### IsString

```cpp
bool IsString() const;
```

Returns a `bool` indicating if this Value is a JavaScript string.

### IsSymbol

```cpp
bool IsSymbol() const;
```

Returns a `bool` indicating if this Value is a JavaScript symbol.

### IsArray

```cpp
bool IsArray() const;
```

Returns a `bool` indicating if this Value is a JavaScript array.

### IsArrayBuffer

```cpp
bool IsArrayBuffer() const;
```

Returns a `bool` indicating if this Value is a JavaScript array buffer.

### IsTypedArray

```cpp
bool IsTypedArray() const;
```

Returns a `bool` indicating if this Value is a JavaScript typed array.

### IsObject

```cpp
bool IsObject() const;
```

Returns a `bool` indicating if this Value is JavaScript object.

### IsFunction

```cpp
bool IsFunction() const;
```

Returns a `bool` indicating if this Value is a JavaScript function.

### IsBuffer

```cpp
bool IsBuffer() const;
```

Returns a `bool` indicating if this Value is a Node buffer.

### As

```cpp
template <typename T> T As() const;
```

Casts to another type of `Napi::Value`, when the actual type is known or assumed.

This conversion does not coerce the type. Calling any methods inappropriate for the actual value type will throw `Napi::Error`.

### ToBoolean

```cpp
Boolean ToBoolean() const;
```

Returns the Value coerced to a JavaScript boolean.

### ToNumber

```cpp
Number ToNumber() const;
```

Returns the Value coerced to a JavaScript number.

### ToString

```cpp
String ToString() const;
```

Returns the Value coerced to a JavaScript string.

### ToObject

```cpp
Object ToObject() const;
```

Returns the Value coerced to a JavaScript object.
