# Function

The **Function** class is a representation of the JavaScript function. A function
is a JavaScript procedure, a set of statements that performs a task or calculates
a value. This class provides some methods to create and exectue a function.

The `Function` class inherits its behavior from the `Object` class (for more info
see: [Object](object.md)).

## Methods

### Constructor

Creates a new empty instance of `Function`.

```cpp
Function();
```

### Constructor

Creates a new instance of `Function` object.

```cpp
Function(napi_env env, napi_value value);
```

- `[in] env`: The `napi_env` environment in which to construct the Value object.
- `[in] value`: The C++ primitive from which to instantiate the Value. `value`
may be any of:

  - bool
  - Any integer type
  - Any floating point type
  - const char* (encoded using UTF-8, null-terminated)
  - const char16_t* (encoded using UTF-16-LE, null-terminated)
  - std::string (encoded using UTF-8)
  - std::u16string
  - napi::Value
  - napi_value

Returns a non-empty `Function` instance.

### New

Creates instance of a `Function` object.

```cpp
template <typename Callable>
static Function New(napi_env env, Callable cb, const char* utf8name = nullptr, void* data = nullptr);
```

- `[in] env`: The `napi_env` environment in which to construct the Function object.
- `[in] cb`: Object that implement the operator **()** accepting a `const CallbackInfo&`
and returns either void or Value.
- `[in] utf8name`: Null-terminated string to be used as the name of the function.
- `[in] data`: User-provided data context. This will be passed back into the
function when invoked later.

Returns instance of a `Function` object.

### New

```cpp
template <typename Callable>
static Function New(napi_env env, Callable cb, const std::string& utf8name, void* data = nullptr);
```

- `[in] env`: The `napi_env` environment in which to construct the Function object.
- `[in] cb`: Object that implement the operator **()** accepting a `const CallbackInfo&`
and returns either void or Value.
- `[in] utf8name`: String to be used as the name of the function.
- `[in] data`: User-provided data context. This will be passed back into the
function when invoked later.

Returns instance of a `Function` object.

### Call

Calls a Javascript function from a native add-on.

```cpp
Value Call(const std::initializer_list<napi_value>& args) const;
```

- `[in] args`: Initializer list of JavaScript values as `napi_value` representing
the arguments of the function.

Returns a Value representing the JavaScript object returned by the function.

### Call

Calls a Javascript function from a native add-on.

```cpp
Value Call(const std::vector<napi_value>& args) const;
```

- `[in] args`: List of JavaScript values as `napi_value` representing the
arguments of the function.

Returns a Value representing the JavaScript object returned by the function.

### Call

Calls a Javascript function from a native add-on.

```cpp
Value Call(size_t argc, const napi_value* args) const;
```

- `[in] argc`: The number of the arguments passed to the function.
- `[in] args`: Array of JavaScript values as `napi_value` representing the
arguments of the function.

Returns a Value representing the JavaScript object returned by the function.

### Call

Calls a Javascript function from a native add-on.

```cpp
Value Call(napi_value recv, const std::initializer_list<napi_value>& args) const;
```

- `[in] recv`: The `this` object passed to the called function.
- `[in] args`: Initializer list of JavaScript values as `napi_value` representing
the arguments of the function.

Returns a Value representing the JavaScript object returned by the function.

### Call

Calls a Javascript function from a native add-on.

```cpp
Value Call(napi_value recv, const std::vector<napi_value>& args) const;
```

- `[in] recv`: The `this` object passed to the called function.
- `[in] args`: List of JavaScript values as `napi_value` representing the
arguments of the function.

Returns a Value representing the JavaScript object returned by the function.

### Call

Calls a Javascript function from a native add-on.

```cpp
Value Call(napi_value recv, size_t argc, const napi_value* args) const;
```

- `[in] recv`: The `this` object passed to the called function.
- `[in] argc`: The number of the arguments passed to the function.
- `[in] args`: Array of JavaScript values as `napi_value` representing the
arguments of the function.

Returns a Value representing the JavaScript object returned by the function.

### MakeCallback

Calls a Javascript function from a native add-on after an asynchronous operation.

```cpp
Value MakeCallback(napi_value recv, const std::initializer_list<napi_value>& args) const;
```

- `[in] recv`: The `this` object passed to the called function.
- `[in] args`: Initializer list of JavaScript values as `napi_value` representing
the arguments of the function.

Returns a Value representing the JavaScript object returned by the function.

### MakeCallback

Calls a Javascript function from a native add-on after an asynchronous operation.

```cpp
Value MakeCallback(napi_value recv, const std::vector<napi_value>& args) const;
```

- `[in] recv`: The `this` object passed to the called function.
- `[in] args`: List of JavaScript values as `napi_value` representing the
arguments of the function.

Returns a Value representing the JavaScript object returned by the function.

### MakeCallback

Calls a Javascript function from a native add-on after an asynchronous operation.

```cpp
Value MakeCallback(napi_value recv, size_t argc, const napi_value* args) const;
```

- `[in] recv`: The `this` object passed to the called function.
- `[in] argc`: The number of the arguments passed to the function.
- `[in] args`: Array of JavaScript values as `napi_value` representing the
arguments of the function.

Returns a Value representing the JavaScript object returned by the function.

### New

Creates a new JavaScript value from one that represents the constructor for the
object.

```cpp
Object New(const std::initializer_list<napi_value>& args) const;
```

- `[in] args`: Initializer list of JavaScript values as `napi_value` representing
the arguments of the contructor function.

Returns a new JavaScript object.

### New

Creates a new JavaScript value from one that represents the constructor for the
object.

```cpp
Object New(const std::vector<napi_value>& args) const;
```

- `[in] args`: List of JavaScript values as `napi_value` representing the
arguments of the constructor function.

Returns a new JavaScript object.

### New

Creates a new JavaScript value from one that represents the constructor for the
object.

```cpp
Object New(size_t argc, const napi_value* args) const;
```

- `[in] argc`: The number of the arguments passed to the contructor function.
- `[in] args`: Array of JavaScript values as `napi_value` representing the
arguments of the constructor function.

Returns a new JavaScript object.

## Operator

```cpp
Value operator ()(const std::initializer_list<napi_value>& args) const;
```

- `[in] args`: Initializer list of JavaScript values as `napi_value`.

Returns a Value representing the JavaScript object returned by the function.
