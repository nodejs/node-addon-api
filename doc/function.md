# Function

N-API provides a set of APIs that allow JavaScript code to call back into native code. N-API APIs that support calling back into native code take in a callback functions represented by the napi_callback type. When the JavaScript VM calls back to native code, the napi_callback function provided is invoked. The APIs documented in this section allow the callback function to do the following:

This API returns an N-API value corresponding to a JavaScript Function object. It's used to wrap native functions so that they can be invoked from JavaScript.

JavaScript Functions are described in Section 19.2 of the ECMAScript Language Specification.

In JavaScript, functions are first-class objects, because they can have properties and methods just like any other object. What distinguishes them from other objects is that functions can be called. In brief, they are Function objects.

Node Addon API consists of a few fundamental data types. These allow a user of the API to create, convert and introspect fundamental JavaScript types, and interop with their C++ counterparts.

Value
Value is the base class of Node Addon API's fundamental object type hierarchy. It represents a JavaScript value of an unknown type. It is a thin wrapper around the N-API datatype napi_value. Methods on this class can be used to check the JavaScript type of the underlying napi_value and also convert to C++ types.

## Methods

### Constructor

Creates a new

```cpp
Function();
```

- `[in] Env`: The environment in which to construct the

Returns a new

### Constructor

Creates a new

```cpp
Function(napi_env env, napi_value value);
```

- `[in] Env`: The environment in which to construct the

Returns a new

### Call

Creates a new

```cpp
Value Call(const std::initializer_list<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### Call

Creates a new

```cpp
Value Call(const std::vector<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### Call

Creates a new

```cpp
Value Call(size_t argc, const napi_value* args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### Call

Creates a new

```cpp
Value Call(napi_value recv, const std::initializer_list<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### Call

Creates a new

```cpp
Value Call(napi_value recv, const std::vector<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### Call

Creates a new

```cpp
Value Call(napi_value recv, size_t argc, const napi_value* args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### MakeCallback

Creates a new

```cpp
Value MakeCallback(napi_value recv, const std::initializer_list<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### MakeCallback

Creates a new

```cpp
Value MakeCallback(napi_value recv, const std::vector<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

### MakeCallback

Creates a new

```cpp
Value MakeCallback(napi_value recv, const std::vector<napi_value>& args) const;
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

### New

Creates a new

```cpp
Object New(size_t argc, const napi_value* args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new

## Operator

```cpp
Value operator ()(const std::initializer_list<napi_value>& args) const;
```

- `[in] Env`: The environment in which to construct the

Returns a new
