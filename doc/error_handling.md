# Error handling

The error handling represents one of the most important thing on implementing a
Node.js native add-on. When an error occurs in your C++ code you have to handle
and dispatch it correctly. **N-API** uses return values and JavaScript exceptions
for error handling. You can choice one method or other based on your preferences
or on the C/C++, library that you are integrating.

The **Error** is a persistent reference (for more info see: [Object reference](object_reference.md)
section) to a JavaScript error object. Use of this class depends somewhat on
whether C++ exceptions are enabled at compile time.

The following sections explain the approach for each case:

- [Handling Errors With C++ Exceptions](#exceptions)

- [Handling Errors Without C++ Exceptions](#noexceptions)

<a name="exceptions"></a>

## Handling Errors With C++ Exceptions

If C++ exceptions are enabled (for more info see: [Setup](setup.md) section),
then the **Error** class extends ```std::exception``` and enables integrated
error-handling for C++ exceptions and JavaScript exceptions.

If a N-API call fails without executing any JavaScript code (for example due to
an invalid argument), then the N-API wrapper automatically converts and throws
the error as a C++ exception of type **Error**.

If a JavaScript function called by C++ code via N-API throws a JavaScript
exception, then the N-API wrapper automatically converts and throws it as a C++
exception of type **Error**.

If a C++ exception of type **Error** escapes from a N-API C++ callback, then
the N-API wrapper automatically converts and throws it as a JavaScript exception.

Therefore, catching a C++ exception of type **Error** prevents a JavaScript
exception from being thrown.

## Examples with C++ exceptions enabled

### Throwing a C++ exception

```cpp
Napi::Env env = ...
throw Napi::Error::New(env, "Example exception");
// other C++ statements
// ...
```

Following C++ statements will not be executed. The exception will bubble up as a
C++ exception of type **Error**, until it is either caught while still in C++, or
else automatically propataged as a JavaScript exception when returns to
JavaScript.

### Propagating a N-API C++ exception

```cpp
Napi::Function jsFunctionThatThrows = someObj.As<Napi::Function>();
Napi::Value result = jsFunctionThatThrows({ arg1, arg2 });
// other C++ statements
// ...
```

Following C++ statements will not be executed. The exception will bubble up as a
C++ exception of type **Error**, until it is either caught while still in C++, or
else automatically propagated as a JavaScript exception when returns to 
JavaScript.

### Handling a N-API C++ exception

```cpp
Napi::Function jsFunctionThatThrows = someObj.As<Napi::Function>();
Napi::Value result;
try {
    result = jsFunctionThatThrows({ arg1, arg2 });
} catch (const Napi::Error& e) {
    cerr << "Caught JavaScript exception: " + e.what();
}
```

Since the exception was caught here, it will not be propagated as a JavaScript
exception.

<a name="noexceptions"></a>

## Handling Errors Without C++ Exceptions

If C++ exceptions are disabled (for more info see: [Setup](setup.md) section),
then the **Error**  class does not extend ```std::exception```. This means that
any call to node-addon-api functions does not throw C++ exception.
Instead, it raises _pending_ JavaScript exceptions and return _empty_ **Value**.
The calling code should check ```Value::IsEmpty()``` (for more info see: [Value](value.md))
before attempting or use a returned value, and may use methods on the **Env** class
to check for, get, and clear a pending JavaScript exception (for more info see: [Env](env.md)).
If the pending exception is not cleared, it will be thrown when the native code
returns to JavaScript.

## Examples with C++ exceptions disabled

### Throwing a JS exception

```cpp
Napi::Env env = ...
Napi::Error::New(env, "Example exception").ThrowAsJavaScriptException();
return;
```

After throwing a JS exception, the code should generally return immediately from
the native callback, after performing any necessary cleanup.

### Propagating a N-API JS exception

```cpp
Napi::Function jsFunctionThatThrows = someObj.As<Napi::Function>();
Napi::Value result = jsFunctionThatThrows({ arg1, arg2 });
if (result.IsEmpty()) return;
```

An empty value result from a N-API call indicates that an error occurred, and a
JavaScript exception is pending. To let the exception propagate, the code should
generally return immediately from the native callback, after performing any
necessary cleanup.

### Handling a N-API JS exception

```cpp
Napi::Function jsFunctionThatThrows = someObj.As<Napi::Function>();
Napi::Value result = jsFunctionThatThrows({ arg1, arg2 });
if (result.IsEmpty()) {
    Napi::Error e = env.GetAndClearPendingException();
    cerr << "Caught JavaScript exception: " + e.Message();
}
```

Since the exception was cleared here, it will not be propagated as a JavaScript
exception after the native callback returns.