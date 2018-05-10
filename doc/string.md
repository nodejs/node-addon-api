# String

## Constructor

```cpp
String();
```

Creates a new **empty** String instance. If an error occurs, a `Napi::Error` 
will get thrown.

```
String(napi_env env, napi_value value); ///< Wraps a N-API value primitive.
```
- `[in] env` - The environment in which to create the string.
- `[in] value` - The primitive to wrap

Wraps a `napi_value` as a `Napi::String`. If an error occurs, a `Napi::Error` 
will get thrown.

## Operators

### operator std::string

```cpp
operator std::string() const;
```
Converts a String value to a UTF-8 encoded C++ string.

### operator std::u16string
```cpp
operator std::u16string() const;
```
Converts a String value to a UTF-16 encoded C++ string.

## Methods

### New
```cpp
String::New();
```
Creates a new empty String

### New
```cpp
String::New(napi_env env, const std::string& value);
String::New(napi_env env, const std::u16string& value);
String::New(napi_env env, const char* value);
String::New(napi_env env, const char16_t* value);
```

- `[in] env`: The `napi_env` environment in which to construct the Value object.
- `[in] value`: The C++ primitive from which to instantiate the Value. `value` may be any of:
  - std::string& - represents an ANSI string
  - std::u16string& - represents a UTF16-LE string
  - const char* - represents a UTF8 string
  - const char16_t* - represents a UTF16-LE string.

Returns a new `Napi::String` that represents the passed in C++ string. If an 
error occurs during the construction of the string, a `Napi::Error` is thrown.

### Utf8Value
```cpp
std::string Utf8Value() const;
```
Converts a String value to a UTF-8 encoded C++ string.

### Utf16Value
```cpp
std::u16string Utf16Value() const;
```

Converts a String value to a UTF-16 encoded C++ string.