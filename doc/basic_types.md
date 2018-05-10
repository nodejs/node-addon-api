# Basic Types

Node Addon API consists of a few fundamental data types. These allow a user of
the API to create, convert and introspect fundamental JavaScript types, and
interoperate with their C++ counterparts.

## Value

Value is the base class of Node Addon API's fundamental object type hierarchy.
It represents a JavaScript value of an unknown type. It is a thin wrapper around
the N-API datatype `napi_value`. Methods on this class can be used to check
the JavaScript type of the underlying N-API `napi_value` and also to convert to
C++ types.

### Constructor

```cpp
Value();
```

Used to create a Node Addon API `Value` that represents an **empty** value.

```cpp
Value(napi_env env, napi_value value);
```

- `[in] env` - The `napi_env` environment in which to construct the Value
object.
- `[in] value` - The underlying JavaScript value that the `Value` instance
represents.

Returns a Node.js Addon API `Value` that represents the `napi_value` passed
in.

### Operators

#### operator napi_value

```cpp
operator napi_value() const;
```

Returns the underlying N-API `napi_value`. If the instance is _empty_, this
returns `nullptr`.

#### operator ==

```cpp
bool operator ==(const Value& other) const;
```

Returns `true` if this value strictly equals another value, or `false` otherwise.

#### operator !=

```cpp
bool operator !=(const Value& other) const;
```

Returns `false` if this value strictly equals another value, or `true` otherwise.

### Methods

#### From
```cpp
template <typename T>
static Value From(napi_env env, const T& value);
```

- `[in] env` - The `napi_env` environment in which to construct the Value object.
- `[in] value` - The C++ type to represent in JavaScript.

Returns a `Napi::Value` representing the input C++ type in JavaScript.

This method is used to convert from a C++ type to a JavaScript value.
Here, `value` may be any of:
- `bool` - returns a `Napi::Boolean`.
- Any integer type - returns a `Napi::Number`.
- Any floating point type - returns a `Napi::Number`.
- `const char*` (encoded using UTF-8, null-terminated) - returns a `Napi::String`.
- `const char16_t*` (encoded using UTF-16-LE, null-terminated) - returns a `Napi::String`.
- `std::string` (encoded using UTF-8) - returns a `Napi::String`.
- `std::u16string` - returns a `Napi::String`.
- `napi::Value` - returns a `Napi::Value`.
- `napi_value` - returns a `Napi::Value`.

#### As
```cpp
template <typename T> T As() const;
```

Returns the `Napi::Value` cast to a desired C++ type.

Use this when the actual type is known or assumed.

Note:
This conversion does NOT coerce the type. Calling any methods inappropriate for
the actual value type will throw `Napi::Error`.

#### StrictEquals
```cpp
bool StrictEquals(const Value& other) const;
```

- `[in] other` - The value to compare against.

Returns true if the other `Napi::Value` is strictly equal to this one.

#### Env
```cpp
Napi::Env Env() const;
```

Returns the environment that the value is associated with. See
[`Napi::Env`](env.md) for more details about environments.

#### IsEmpty
```cpp
bool IsEmpty() const;
```

Returns `true` if the value is uninitialized.

An empty value is invalid, and most attempts to perform an operation on an
empty value will result in an exception. An empty value is distinct from
JavaScript `null` or `undefined`, which are valid values.

When C++ exceptions are disabled at compile time, a method with a `Value`
return type may return an empty value to indicate a pending exception. If C++
exceptions are not being used, callers should check the result of
`Env::IsExceptionPending` before attempting to use the value.

#### Type
```cpp
napi_valuetype Type() const;
```

Returns the underlying N-API `napi_valuetype` of the value.

#### IsUndefined
```cpp
bool IsUndefined() const;
```

Returns `true` if the underlying value is a JavaScript `undefined` or `false`
otherwise.

#### IsNull
```cpp
bool IsNull() const;
```

Returns `true` if the underlying value is a JavaScript `null` or `false`
otherwise.

#### IsBoolean
```cpp
bool IsBoolean() const;
```

Returns `true` if the underlying value is a JavaScript `true` or JavaScript
`false`, or `false` if the value is not a Boolean value in JavaScript.

#### IsNumber
```cpp
bool IsNumber() const;
```

Returns `true` if the underlying value is a JavaScript `Number` or `false`
otherwise.

#### IsString
```cpp
bool IsString() const;
```

Returns `true` if the underlying value is a JavaScript `String` or `false`
otherwise.

#### IsSymbol
```cpp
bool IsSymbol() const;
```

Returns `true` if the underlying value is a JavaScript `Symbol` or `false`
otherwise.

#### IsArray
```cpp
bool IsArray() const;
```

Returns `true` if the underlying value is a JavaScript `Array` or `false`
otherwise.

#### IsArrayBuffer
```cpp
bool IsArrayBuffer() const;
```

Returns `true` if the underlying value is a JavaScript `ArrayBuffer` or `false`
otherwise.

#### IsTypedArray
```cpp
bool IsTypedArray() const;
```

Returns `true` if the underlying value is a JavaScript `TypedArray` or `false`
otherwise.

#### IsObject
```cpp
bool IsObject() const;
```

Returns `true` if the underlying value is a JavaScript `Object` or `false`
otherwise.

#### IsFunction
```cpp
bool IsFunction() const;
```

Returns `true` if the underlying value is a JavaScript `Function` or `false`
otherwise.

#### IsPromise
```cpp
bool IsPromise() const;
```

Returns `true` if the underlying value is a JavaScript `Promise` or `false`
otherwise.

#### IsDataView
```cpp
bool IsDataView() const;
```

Returns `true` if the underlying value is a JavaScript `DataView` or `false`
otherwise.

#### IsBuffer
```cpp
bool IsBuffer() const;
```

Returns `true` if the underlying value is a Node.js `Buffer` or `false`
otherwise.

#### IsExternal
```cpp
bool IsExternal() const;
```

Returns `true` if the underlying value is a N-API external object or `false`
otherwise.

#### ToBoolean
```cpp
Boolean ToBoolean() const;
```

Returns a `Napi::Boolean` representing the `Napi::Value`.

This is a wrapper around `napi_coerce_to_boolean`. This will throw a JavaScript
exception if the coercion fails. If C++ exceptions are not being used, callers
should check the result of `Env::IsExceptionPending` before attempting to use
the returned value.


#### ToNumber
```cpp
Number ToNumber() const;
```

Returns a `Napi::Number` representing the `Napi::Value`.

Note:
This can cause script code to be executed according to JavaScript semantics.
This is a wrapper around `napi_coerce_to_number`. This will throw a JavaScript
exception if the coercion fails. If C++ exceptions are not being used, callers
should check the result of `Env::IsExceptionPending` before attempting to use
the returned value.


#### ToString
```cpp
String ToString() const;
```

Returns a `Napi::String` representing the `Napi::Value`.

Note that this can cause script code to be executed according to JavaScript
semantics. This is a wrapper around `napi_coerce_to_string`. This will throw a
JavaScript exception if the coercion fails. If C++ exceptions are not being
used, callers should check the result of `Env::IsExceptionPending` before
attempting to use the returned value.


#### ToObject
```cpp
Object ToObject() const;
```

Returns a `Napi::Object` representing the `Napi::Value`.

This is a wrapper around `napi_coerce_to_object`. This will throw a JavaScript
exception if the coercion fails. If C++ exceptions are not being used, callers
should check the result of `Env::IsExceptionPending` before attempting to use
the returned value.


## Name

Names are JavaScript values that can be used as a property name. There are two
specialized types of names supported in Node.js Addon API- [`String`](String.md)
and [`Symbol`](Symbol.md).

### Methods

#### Constructor
```cpp
Name();
```

Returns an empty `Name`.

```cpp
Name(napi_env env, napi_value value);
```
- `[in] env` - The environment in which to create the array.
- `[in] value` - The primitive to wrap.

Returns a Name created from the JavaScript primitive.

Note:
The value is not coerced to a string.

## Array

Arrays are native representations of JavaScript Arrays. `Napi::Array` is a wrapper
around `napi_value` representing a JavaScript Array.

### Constructor
```cpp
Array();
```

Returns an empty array.

If an error occurs, a `Napi::Error` will be thrown. If C++ exceptions are not
being used, callers should check the result of `Env::IsExceptionPending` before
attempting to use the returned value.


```cpp
Array(napi_env env, napi_value value);
```
- `[in] env` - The environment in which to create the array.
- `[in] value` - The primitive to wrap.

Returns a `Napi::Array` wrapping a `napi_value`.

If an error occurs, a `Napi::Error` will get thrown. If C++ exceptions are not
being used, callers should check the result of `Env::IsExceptionPending` before
attempting to use the returned value.

### Methods

#### New
```cpp
static Array New(napi_env env);
```
- `[in] env` - The environment in which to create the array.

Returns a new `Napi::Array`.

If an error occurs, a `Napi::Error` will get thrown. If C++ exceptions are not
being used, callers should check the result of `Env::IsExceptionPending` before
attempting to use the returned value.

#### New

```cpp
static Array New(napi_env env, size_t length);
```
- `[in] env` - The environment in which to create the array.
- `[in] length` - The length of the array.

Returns a new `Napi::Array` with the given length.

If an error occurs, a `Napi::Error` will get thrown. If C++ exceptions are not
being used, callers should check the result of `Env::IsExceptionPending` before
attempting to use the returned value.

#### New
```cpp
uint32_t Length() const;
```

Returns the length of the array.

Note:
This can execute JavaScript code implicitly according to JavaScript semantics.
If an error occurs, a `Napi::Error` will get thrown. If C++ exceptions are not
being used, callers should check the result of `Env::IsExceptionPending` before
attempting to use the returned value.
