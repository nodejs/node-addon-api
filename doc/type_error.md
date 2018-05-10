# TypeError

The **TypeError** class is a representation of the JavaScript TypeError that is
thrown when an operand or argument passed to a function is incompatible with the
type expected by the operator or function.

The **TypeError** class inherits its behaviours from **Error** class (for more info
see: [Error](error.md) section).

For more details about error handling refer to the section titled [Error handling](error_handling.md).

## Methods

### New

Creates a new instance of `TypeError` object

```cpp
TypeError::New(Napi:Env env, const char* message);
```

- `[in] Env`: The environment in which to construct the `TypeError` object.
- `[in] message`: Null-terminated strings to be used as the message for the `TypeError`.

Returns an instance of `TypeError` object.

### New

Creates a new instance of `TypeError` object

```cpp
TypeError::New(Napi:Env env, const std::string& message);
```

- `[in] Env`: The environment in which to construct the `TypeError` object.
- `[in] message`: Reference string to be used as the message for the `TypeError`.

Returns an instance of `TypeError` object.

### Constructor

Creates a new empty instance of `TypeError`

```cpp
TypeError();
```

### Constructor

Initializes a ```TypeError``` instance from an existing JavaScript error object.

```cpp
TypeError(napi_env env, napi_value value);
```

- `[in] Env`: The environment in which to construct the `TypeError` object.
- `[in] value`: The `Error` reference to wrap.

Returns an instance of `TypeError` object.