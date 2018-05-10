# Symbol

## Methods

### Constructor

Instantiates a new `Symbol` value

```cpp
Symbol();
```

Returns a new empty Symbol.

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
  - `std::string&` - ANSI string description.
  - `const char*` - represents a UTF8 string description.
  - `String` - Node addon API String description.
  - `napi_value` - N-API `napi_value` description.

If an error occurs, a `Napi::Error` will get thrown. If C++ exceptions are not
being used, callers should check the result of `Env::IsExceptionPending` before
attempting to use the returned value.

### Utf8Value
```cpp
static Symbol WellKnown(napi_env env, const std::string& name);
```

- `[in] env`: The `napi_env` environment in which to construct the Symbol object.
- `[in] name`: The C++ string representing the `Symbol` to retrieve.

Returns a `Napi::Symbol` representing a well-known Symbol from the
Symbol registry.
