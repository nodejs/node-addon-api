# Env

Class `Napi::Env` inherits from class [`Napi::BasicEnv`][].

The data structure containing the environment in which the request is being run.

The `Napi::Env` object is usually created and passed by the Node.js runtime or
node-addon-api infrastructure.

The `Napi::Env` object represents an environment that has a superset of APIs
when compared to `Napi::BasicEnv` and therefore _cannot_ be used in basic
finalizers. See [Finalization][] for more details.

## Methods

### Constructor

```cpp
Napi::Env::Env(napi_env env);
```

- `[in] env`: The `napi_env` environment from which to construct the `Napi::Env` object.

### napi_env

```cpp
operator napi_env() const;
```

Returns the `napi_env` opaque data structure representing the environment.

### Global

```cpp
Napi::Object Napi::Env::Global() const;
```

Returns the `Napi::Object` representing the environment's JavaScript Global Object.

### Undefined

```cpp
Napi::Value Napi::Env::Undefined() const;
```

Returns the `Napi::Value` representing the environment's JavaScript Undefined Object.

### Null

```cpp
Napi::Value Napi::Env::Null() const;
```

Returns the `Napi::Value` representing the environment's JavaScript Null Object.

### IsExceptionPending

```cpp
bool Napi::Env::IsExceptionPending() const;
```

Returns a `bool` indicating if an exception is pending in the environment.

### GetAndClearPendingException

```cpp
Napi::Error Napi::Env::GetAndClearPendingException() const;
```

Returns an `Napi::Error` object representing the environment's pending exception, if any.

### RunScript

```cpp
Napi::Value Napi::Env::RunScript(____ script) const;
```
- `[in] script`: A string containing JavaScript code to execute.

Runs JavaScript code contained in a string and returns its result.

The `script` can be any of the following types:
- [`Napi::String`](string.md)
- `const char *`
- `const std::string &`

[`Napi::BasicEnv`]: ./basic_env.md
[Finalization]: ./finalization.md
