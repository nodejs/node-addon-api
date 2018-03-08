# Error handling

A persistent reference to a JavaScript error object. Use of this class depends 
somewhat on whether C++ exceptions are enabled at compile time.

### Handling Errors With C++ Exceptions

If C++ exceptions are enabled, then the `Error` class extends `std::exception` 
and enables integrated error-handling for C++ exceptions and JavaScript 
exceptions.

If a N-API call fails without executing any JavaScript code (for example due to 
an invalid argument), then the N-API wrapper automatically converts and throws 
the error as a C++ exception of type `Napi::Error`. Or if a JavaScript function 
called by C++ code via N-API throws a JavaScript exception, then the N-API 
wrapper automatically converts and throws it as a C++ exception of type 
`Napi::Error`.

If a C++ exception of type `Napi::Error` escapes from a N-API C++ callback, then 
the N-API wrapper automatically converts and throws it as a JavaScript exception. 
Therefore, catching a C++ exception of type `Napi::Error` prevents a JavaScript 
exception from being thrown.

#### Example 1A - Throwing a C++ exception:

```cpp
Napi::Env env = ...
throw Napi::Error::New(env, "Example exception");
```

Following C++ statements will not be executed. The exception will bubble up as a 
C++ exception of type `Napi::Error`, until it is either caught while still in 
C++, or else automatically propataged as a JavaScript exception when the callback
returns to JavaScript.

#### Example 2A - Propagating a N-API C++ exception:

```cpp
Napi::Function jsFunctionThatThrows = someObj.As<Napi::Function>();
Napi::Value result = jsFunctionThatThrows({ arg1, arg2 });
```

Following C++ statements will not be executed. The exception will bubble up as a 
C++ exception of type `Napi::Error`, until it is either caught while still in 
C++, or else automatically propagated as a JavaScript exception when the callback 
returns to JavaScript.

#### Example 3A - Handling a N-API C++ exception:

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

### Handling Errors Without C++ Exceptions

If C++ exceptions are disabled (by defining `NAPI_DISABLE_CPP_EXCEPTIONS`) then 
this class does not extend `std::exception`, and APIs in the `Napi` namespace do 
not throw C++ exceptions when they fail. Instead, they raise _pending_ JavaScript
exceptions and return _empty_ `Value`s. Calling code should check 
`Value::IsEmpty()` before attempting o use a returned value, and may use methods 
on the `Env` class to check for, get, and clear a pending JavaScript exception. 
If the pending exception is not cleared, it will be thrown when the native 
callback returns to JavaScript.

#### Example 1B - Throwing a JS exception

```cpp
Napi::Env env = ...
Napi::Error::New(env, "Example exception").ThrowAsJavaScriptException();
return;
```

After throwing a JS exception, the code should generally return immediately from 
the native callback, after performing any necessary cleanup.

#### Example 2B - Propagating a N-API JS exception:

```cpp
Napi::Function jsFunctionThatThrows = someObj.As<Napi::Function>();
Napi::Value result = jsFunctionThatThrows({ arg1, arg2 });
if (result.IsEmpty()) return;
```

An empty value result from a N-API call indicates an error occurred, and a 
JavaScript xception is pending. To let the exception propagate, the code should 
generally return immediately from the native callback, after performing any 
necessary cleanup.

#### Example 3B - Handling a N-API JS exception:

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