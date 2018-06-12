# TypeError

The **TypeError** class is a representation of the JavaScript `TypeError` that is
thrown when an operand or argument passed to a function is incompatible with the
type expected by the operator or function.

The **TypeError** class inherits its behaviors from the **Error** class (for more info
see: [Error](error.md)).

For more details about error handling refer to the section titled [Error handling](error_handling.md).

## Methods

### New

Creates a new instance of the `TypeError` object.

```cpp
TypeError::New(Napi:Env env, const char* message);
```

- `[in] Env`: The environment in which to construct the `TypeError` object.
- `[in] message`: Null-terminated string to be used as the message for the `TypeError`.

Returns an instance of a `TypeError` object.

### New

Creates a new instance of a `TypeError` object.

```cpp
TypeError::New(Napi:Env env, const std::string& message);
```

- `[in] Env`: The environment in which to construct the `TypeError` object.
- `[in] message`: Reference string to be used as the message for the `TypeError`.

Returns an instance of a `TypeError` object.

### Constructor

Creates a new empty instance of a `TypeError`.

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

Returns an instance of a `TypeError` object.