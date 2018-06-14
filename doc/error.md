# Error

The **Error** class is a representation of the JavaScript Error object that is thrown
when runtime errors occur. The Error object can also be used as a base object for
user-defined exceptions.

The **Error** class is a persistent reference to a JavaScript error object thus
inherits its behavior from the `ObjectReference` class (for more info see: [ObjectReference](object_reference.md)).

If C++ exceptions are enabled (for more info see: [Setup](setup.md)), then the
**Error** class extends `std::exception` and enables integrated
error-handling for C++ exceptions and JavaScript exceptions.

For more details about error handling refer to the section titled [Error handling](error_handling.md).

## Methods

### New

Creates empty instance of an `Error` object for the specified environment.

```cpp
Error::New(Napi:Env env);
```

- `[in] Env`: The environment in which to construct the Error object.

Returns an instance of `Error` object.

### New

Creates instance of an `Error` object.

```cpp
Error::New(Napi:Env env, const char* message);
```

- `[in] Env`: The environment in which to construct the Error object.
- `[in] message`: Null-terminated string to be used as the message for the Error.

Returns instance of an `Error` object.

### New

Creates instance of an `Error` object

```cpp
Error::New(Napi:Env env, const std::string& message);
```

- `[in] Env`: The environment in which to construct the `Error` object.
- `[in] message`: Reference string to be used as the message for the `Error`.

Returns instance of an `Error` object.

### Fatal

In case of an unrecoverable error in a native module, a fatal error can be thrown
to immediately terminate the process.

```cpp
static NAPI_NO_RETURN void Fatal(const char* location, const char* message);
```

The function call does not return, the process will be terminated.

### Constructor

Creates empty instance of an `Error`.

```cpp
Error();
```

Returns an instance of `Error` object.

### Constructor

Initializes an `Error` instance from an existing JavaScript error object.

```cpp
Error(napi_env env, napi_value value);
```

- `[in] Env`: The environment in which to construct the Error object.
- `[in] value`: The `Error` reference to wrap.

Returns instance of an `Error` object.

### Message

```cpp
std::string& Message() const NAPI_NOEXCEPT;
```

Returns the reference to the string that represent the message of the error.

### ThrowAsJavaScriptException

Throw the error as JavaScript exception.

```cpp
void ThrowAsJavaScriptException() const;
```

Throws the error as a JavaScript exception.

### what

```cpp
const char* what() const NAPI_NOEXCEPT override;
```

Returns a pointer to a null-terminated string that is used to identify the
exception. This method can be used only if the exception mechanism is enabled.
