# Boolean

# Methods

### Constructor

```cpp
Napi::Boolean::New(Napi::Env env, bool value);
```
 - `[in] env`: The `napi_env` Environment
 - `[in] value`: The Javascript boolean value

```cpp
Napi::Boolean::Boolean();
```
returns a new empty Javascript Boolean value type.

### operator bool
Converts a `Napi::Boolean` value to a boolean primitive.
```cpp
Napi::Boolean::operator bool() const;
```

### Value
Converts a `Napi::Boolean` value to a boolean primitive.

```cpp
bool Napi::Boolean::Value() const;
```
