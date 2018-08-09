# FunctionReference

`FunctionReference` is a subclass of [Reference](reference.md), and is equivalent to
an instance of `Reference<Function>`. This means that a FunctionReference holds a
[`Function`](function.md), and a count of the number of references to that `Function`.
When the count is greater than 0, a FunctionReference is not eligible for garbage
collection. This ensures that the `Function` will remain accessible, even if the
original reference to it is no longer available.
`FunctionReference` allows the referenced JavaScript function object  to be called
from a native add-on with two different methods: `Call` and `MakeCallback`. See
the documentation for [`Function`](function.md) for when `Call` should be used
instead of `MakeCallback` and vice-versa.

The `FunctionReference` class inherits its behavior from the `Reference` class
(for more info see: [Reference](reference.md)).

## Methods

### Weak

Creates a "weak" reference to the value, in that the initial count of number of
references is set to 0.

```cpp
static FunctionReference Weak(const Function& value);
```

- `[in] value`: The value which is to be referenced.

Returns the newly created reference.

### Persistent

Creates a "persistent" reference to the value, in that the initial count of
number of references is set to 1.

```cpp
static FunctionReference Persistent(const Function& value);
```

- `[in] value`: The value which is to be referenced.

Returns the newly created reference.

### Constructor

Creates a new empty instance of `FunctionReference`.

```cpp
FunctionReference();
```

### Constructor

Creates a new instance of the `FunctionReference`.

```cpp
FunctionReference(napi_env env, napi_ref ref);
```

- `[in] env`: The environment in which to construct the FunctionReference object.
- `[in] ref`: The N-API reference to be held by the FunctionReference.

Returns a newly created `FunctionReference` object.

### New

Constructs a new instance by calling the constructor held by this reference.

```cpp
Napi::Object New(const std::initializer_list<napi_value>& args) const;
```

- `[in] args`: Initializer list of JavaScript values as napi_value representing
the arguments of the contructor function.

Returns a new JavaScript object.

### New

Constructs a new instance by calling the constructor held by this reference.

```cpp
Napi::Object New(const std::vector<napi_value>& args) const;
```

- `[in] args`: Vector of JavaScript values as napi_value representing the
arguments of the constructor function.

Returns a new JavaScript object.

### Call

Calls a referenced Javascript function from a native add-on.

```cpp
Napi::Value Call(const std::initializer_list<napi_value>& args) const;
```

- `[in] args`: Initializer list of JavaScript values as `napi_value` representing
the arguments of the referenced function.

Returns a `Napi::Value` representing the JavaScript object returned by the referenced
function.

### Call

Calls a referenced Javascript function from a native add-on.

```cpp
Napi::Value Call(const std::vector<napi_value>& args) const;
```

- `[in] args`: Vector of JavaScript values as `napi_value` representing the
arguments of the referenced function.

Returns a `Napi::Value` representing the JavaScript object returned by the referenced
function.

### Call

Calls a referenced Javascript function from a native add-on.

```cpp
Napi::Value Call(napi_value recv, const std::initializer_list<napi_value>& args) const;
```

- `[in] recv`: The `this` object passed to the referenced function when it's called.
- `[in] args`: Initializer list of JavaScript values as `napi_value` representing
the arguments of the referenced function.

Returns a `Napi::Value` representing the JavaScript object returned by the referenced
function.

### Call

Calls a referenced Javascript function from a native add-on.

```cpp
Napi::Value Call(napi_value recv, const std::vector<napi_value>& args) const;
```

- `[in] recv`: The `this` object passed to the referenced function when it's called.
- `[in] args`: Vector of JavaScript values as `napi_value` representing the
arguments of the referenced function.

Returns a `Napi::Value` representing the JavaScript object returned by the referenced
function.

### MakeCallback

Calls a referenced Javascript function from a native add-on after an asynchronous
operation.

```cpp
Napi::Value MakeCallback(napi_value recv, const std::initializer_list<napi_value>& args) const;
```

- `[in] recv`: The `this` object passed to the referenced function when it's called.
- `[in] args`: Initializer list of JavaScript values as `napi_value` representing
the arguments of the referenced function.

Returns a `Napi::Value` representing the JavaScript object returned by the referenced
function.

### MakeCallback

Calls a referenced Javascript function from a native add-on after an asynchronous
operation.

```cpp
Napi::Value MakeCallback(napi_value recv, const std::vector<napi_value>& args) const;
```

- `[in] recv`: The `this` object passed to the referenced function when it's called.
- `[in] args`: Vector of JavaScript values as `napi_value` representing the
arguments of the referenced function.

Returns a `Napi::Value` representing the JavaScript object returned by the referenced
function.

## Operator

```cpp
Napi::Value operator ()(const std::initializer_list<napi_value>& args) const;
```

- `[in] args`: Initializer list of reference to JavaScript values as `napi_value`

Returns a `Napi::Value` representing the JavaScript value returned by the referenced
function.
