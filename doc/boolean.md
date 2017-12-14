# Boolean

You are reading a draft of the next documentation and it's in continuous update so
if you don't find what you need please refer to:
[C++ wrapper classes for the ABI-stable C APIs for Node.js](https://nodejs.github.io/node-addon-api/)

# Methods

### Constructor

```cpp
Napi::Boolean::New(Napi::Env env, bool value);
```
 - `[in] env`: The `napi_env` Environment
 - `[in] value`: The Javascript boolean value

```cpp
Napi::Boolean();
```
returns a new empty Javascript Boolean value type.

### operator bool
Converts a Boolean value to a boolean primitive.
```cpp
operator bool() const;
```

### Value
Converts a Boolean value to a boolean primitive.

```cpp
bool Value() const;
```