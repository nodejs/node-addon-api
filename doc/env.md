**WORK IN PROGRESS, NOT YET COMPLETE**

# Env

The opaque data structure containing the environment in which the request is being run.

The Env object is usually created and passed by the Node.js runtime or node-addon-api infrastructure.

## Methods

### Constructor

```cpp
Env(napi_env env);
```

- `[in] env`: The `napi_env` environment from which to construct the `Env` object.

### napi_env

```cpp
operator napi_env() const;
```

Returns the `napi_env` opaque data structure representing the environment.

### Global

```cpp
Object Global() const;
```

Returns the `Object` representing the environment's JavaScript Global Object.

### Undefined

```cpp
Value Undefined() const;
```

Returns the `Value` representing the environment's JavaScript Undefined Object.

### Null

```cpp
Value Null() const;
```

Returns the `Value` representing the environment's JavaScript Null Object.

### IsExceptionPending

```cpp
bool IsExceptionPending() const;
```

Returns a `bool` indicating if an exception is pending in the environment.

### GetAndClearPendingException

```cpp
Error GetAndClearPendingException();
```

Returns an `Error` object representing the environment's pending exception, if any.
