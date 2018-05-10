# Symbol

## Methods

### Constructor

Instantiates a new `Symbol` value

```cpp
Symbol();
```
Creates a new empty Symbol

### New
```cpp
Symbol::New(napi_env env, const std::string& description);
Symbol::New(napi_env env, const char* description);
Symbol::New(napi_env env, String description);
Symbol::New(napi_env env, napi_value description);
```

- `[in] env`: The `napi_env` environment in which to construct the Symbol object.
- `[in] value`: The C++ primitive which represents the description hint for the Symbol. 
  `description` may be any of:
  - std::string& - ANSI string description
  - const char* - represents a UTF8 string description
  - String - Node addon API string description.
  - napi_value - Node addon API string description.

Returns a new `Napi::Symbol` that represents a JavaScript symbol. If an 
error occurs during the construction of the string, a `Napi::Error` is thrown.

### Utf8Value
```cpp
static Symbol WellKnown(napi_env, const std::string& name);
```
Retrieves a well-known symbol from the symbol registry.
