# String

You are reading a draft of the next documentation and it's in continuos update so
if you don't find what you need please refer to:
[C++ wrapper classes for the ABI-stable C APIs for Node.js](https://nodejs.github.io/node-addon-api/)

## Methods

### Constructor

Creates a new String value from a UTF-8 encoded c++ string.

```cpp
String::New(napi_env env, const std::string& value);
```

- `[in] env`: The `napi_env` environment in which to construct the Value object.
- `[in] value`: The C++ primitive from which to instantiate the Value. `value` may be any of:
  - std::string&
  - std::u16string&
  - const char*
  - const char16_t*

Creates a new empty String

```cpp
String::New();
```

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