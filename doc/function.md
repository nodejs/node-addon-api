# Function

The **Function** class provides a set of methods for creating a function object in
native code that can later be called from JavaScript. The created function is not
automatically visible from JavaScript. Instead it needs to be part of the add-on's
module exports or be returned by one of the module's exported functions.

In addition the `Function` class also provides methods that can be used to call
functions that were created in JavaScript and passed to the native  add-on.

The `Function` class inherits its behavior from the `Object` class (for more info
see: [Object](object.md)).

## Example

```cpp
#include <napi.h>

using namespace Napi;

Value Fn(const CallbackInfo& info) {
  Env env = info.Env();
  // ...
  return String::New(env, "Hello World");
}

Object Init(Env env, Object exports) {
  exports.Set(String::New(env, "fn"), Function::New(env, Fn));
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
```

The above code can be used from JavaScript as follows:

```js
const addon = require('./addon');
addon.fn();
```

With the `Function` class it is possible to call a JavaScript function object
from a native add-on with two different methods: `Call` and `MakeCallback`.
The API of these two methods is very similar, but they are used in different
context. The `MakeCallback` method is used to call from native code back into
JavaScript after returning from an [asynchronous operation](async_operations.md)
and in general in situations which don't have an existing JavaScript function on
the stack. The `Call` method is used when there is already a JavaScript function
on the stack (for example when running a native method called from JavaScript).

## Methods

### Constructor

Creates a new empty instance of `Function`.

```cpp
Function();
```

### Constructor

Creates a new instance of the `Function` object.

```cpp
Function(napi_env env, napi_value value);
```

- `[in] env`: The `napi_env` environment in which to construct the Value object.
- `[in] value`: The `napi_value` which is handle for a JavaScript function.

Returns a non-empty `Function` instance.

### New

Creates an instance of a `Function` object.

```cpp
template <typename Callable>
static Function New(napi_env env, Callable cb, const char* utf8name = nullptr, void* data = nullptr);
```

- `[in] env`: The `napi_env` environment in which to construct the Function object.
- `[in] cb`: Object that implements `Callable`.
- `[in] utf8name`: Null-terminated string to be used as the name of the function.
- `[in] data`: User-provided data context. This will be passed back into the
function when invoked later.

Returns an instance of a `Function` object.

### New

```cpp
template <typename Callable>
static Function New(napi_env env, Callable cb, const std::string& utf8name, void* data = nullptr);
```

- `[in] env`: The `napi_env` environment in which to construct the Function object.
- `[in] cb`: Object that implements `Callable`.
- `[in] utf8name`: String to be used as the name of the function.
- `[in] data`: User-provided data context. This will be passed back into the
function when invoked later.

Returns an instance of a `Function` object.

### New

Creates a new JavaScript value from one that represents the constructor for the
object.

```cpp
Napi::Object New(const std::initializer_list<napi_value>& args) const;
```

- `[in] args`: Initializer list of JavaScript values as `napi_value` representing
the arguments of the contructor function.

Returns a new JavaScript object.

### New

Creates a new JavaScript value from one that represents the constructor for the
object.

```cpp
Napi::Object New(const std::vector<napi_value>& args) const;
```

- `[in] args`: Vector of JavaScript values as `napi_value` representing the
arguments of the constructor function.

Returns a new JavaScript object.

### New

Creates a new JavaScript value from one that represents the constructor for the
object.

```cpp
Napi::Object New(size_t argc, const napi_value* args) const;
```

- `[in] argc`: The number of the arguments passed to the contructor function.
- `[in] args`: Array of JavaScript values as `napi_value` representing the
arguments of the constructor function.

Returns a new JavaScript object.

### Call

Calls a Javascript function from a native add-on.

```cpp
Napi::Value Call(const std::initializer_list<napi_value>& args) const;
```

- `[in] args`: Initializer list of JavaScript values as `napi_value` representing
the arguments of the function.

Returns a `Napi::Value` representing the JavaScript value returned by the function.

### Call

Calls a JavaScript function from a native add-on.

```cpp
Napi::Value Call(const std::vector<napi_value>& args) const;
```

- `[in] args`: Vector of JavaScript values as `napi_value` representing the
arguments of the function.

Returns a `Napi::Value` representing the JavaScript value returned by the function.

### Call

Calls a Javascript function from a native add-on.

```cpp
Napi::Value Call(size_t argc, const napi_value* args) const;
```

- `[in] argc`: The number of the arguments passed to the function.
- `[in] args`: Array of JavaScript values as `napi_value` representing the
arguments of the function.

Returns a `Napi::Value` representing the JavaScript value returned by the function.

### Call

Calls a Javascript function from a native add-on.

```cpp
Napi::Value Call(napi_value recv, const std::initializer_list<napi_value>& args) const;
```

- `[in] recv`: The `this` object passed to the called function.
- `[in] args`: Initializer list of JavaScript values as `napi_value` representing
the arguments of the function.

Returns a `Napi::Value` representing the JavaScript value returned by the function.

### Call

Calls a Javascript function from a native add-on.

```cpp
Napi::Value Call(napi_value recv, const std::vector<napi_value>& args) const;
```

- `[in] recv`: The `this` object passed to the called function.
- `[in] args`: Vector of JavaScript values as `napi_value` representing the
arguments of the function.

Returns a `Napi::Value` representing the JavaScript value returned by the function.

### Call

Calls a Javascript function from a native add-on.

```cpp
Napi::Value Call(napi_value recv, size_t argc, const napi_value* args) const;
```

- `[in] recv`: The `this` object passed to the called function.
- `[in] argc`: The number of the arguments passed to the function.
- `[in] args`: Array of JavaScript values as `napi_value` representing the
arguments of the function.

Returns a `Napi::Value` representing the JavaScript value returned by the function.

### MakeCallback

Calls a Javascript function from a native add-on after an asynchronous operation.

```cpp
Napi::Value MakeCallback(napi_value recv, const std::initializer_list<napi_value>& args) const;
```

- `[in] recv`: The `this` object passed to the called function.
- `[in] args`: Initializer list of JavaScript values as `napi_value` representing
the arguments of the function.

Returns a `Napi::Value` representing the JavaScript value returned by the function.

### MakeCallback

Calls a Javascript function from a native add-on after an asynchronous operation.

```cpp
Napi::Value MakeCallback(napi_value recv, const std::vector<napi_value>& args) const;
```

- `[in] recv`: The `this` object passed to the called function.
- `[in] args`: List of JavaScript values as `napi_value` representing the
arguments of the function.

Returns a `Napi::Value` representing the JavaScript value returned by the function.

### MakeCallback

Calls a Javascript function from a native add-on after an asynchronous operation.

```cpp
Napi::Value MakeCallback(napi_value recv, size_t argc, const napi_value* args) const;
```

- `[in] recv`: The `this` object passed to the called function.
- `[in] argc`: The number of the arguments passed to the function.
- `[in] args`: Array of JavaScript values as `napi_value` representing the
arguments of the function.

Returns a `Napi::Value` representing the JavaScript value returned by the function.

## Operator

```cpp
Napi::Value operator ()(const std::initializer_list<napi_value>& args) const;
```

- `[in] args`: Initializer list of JavaScript values as `napi_value`.

Returns a `Napi::Value` representing the JavaScript value returned by the function.
