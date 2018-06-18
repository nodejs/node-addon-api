# FunctionReference

## Methods

### Constructor

Creates a new

```cpp
FunctionReference();
```

- `[in] Env`: The environment in which to construct the

Returns a new

### Constructor

Creates a new

```cpp
FunctionReference(napi_env env, napi_ref ref);
```

- `[in] Env`: The environment in which to construct the

Returns a new

### Call

Creates a new

```cpp
Napi::Value Call(const std::initializer_list<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### Call

Creates a new

```cpp
Napi::Value Call(const std::vector<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### Call

Creates a new

```cpp
Napi::Value Call(napi_value recv, const std::initializer_list<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### Call

Creates a new

```cpp
Napi::Value Call(napi_value recv, const std::vector<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### MakeCallback

Creates a new

```cpp
Napi::Value MakeCallback(napi_value recv, const std::initializer_list<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### MakeCallback

Creates a new

```cpp
Napi::Value MakeCallback(napi_value recv, const std::vector<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### New

Creates a new

```cpp
Object New(const std::initializer_list<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### New

Creates a new

```cpp
Object New(const std::vector<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

## Operator

```cpp
Napi::Value operator ()(const std::initializer_list<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new
