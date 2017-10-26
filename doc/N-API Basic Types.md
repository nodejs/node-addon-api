# N-API Documentation

## Env

The opaque data structure containing the environment in which the request is being run. 

The Env object is usually created and passed by the calling system.

```
Env(napi_env env);
```

- `[in] env`: The `napi_env` environment from which to construct the `Env` object.

```
operator napi_env() const;
```

Returns the `napi_env` opaque data structure representing the environment. 

```
Object Global() const;
```

Returns the `Object` representing JavaScript Global Object.

```
Value Undefined() const;
```

Returns the `Value` representing JavaScript Undefined Object.

```
Value Null() const;
```

Returns the `Value` representing JavaScript Null Object.

```
bool IsExceptionPending() const;
```

Returns a `bool` indicating if an exception is pending.

```
Error GetAndClearPendingException();
```

Returns an `Error` object representing the pending exception, if any.

## Value

A Value is the C++ manifestation of a JavaScript value.

Value is a the base class upon which other JavaScipt values like Number, Boolean, String, and Object are based. 

```
Value();
```

Creates a new *empty* Value instance.

```
Value(napi_env env, napi_value value);
``` 

- `[in] env`: The `napi_env` environment in which to construct the Value object.

- `[in] value`: The C++ primitive from which to instantiate the Value. `value` may be any of:
  - bool
  - Any integer type
  - Any floating point type
  - const char* (encoded using UTF-8, null-terminated)
  - const char16_t* (encoded using UTF-16-LE, null-terminated)
  - std::string (encoded using UTF-8)
  - std::u16string
  - napi::Value
  - napi_value

```
template <typename T> static Value From(napi_env env, const T& value);
```

```
operator napi_value() const;
```

Returns this Value's N-API value primitive.

Returns `nullptr` if this Value is *empty*. 

```
bool operator ==(const Value& other) const;
```

Returns a `bool` indicating if this Value strictly equals another Value.

```
bool operator !=(const Value& other) const;
```

Returns a `bool` indicating if this Value does not strictly equal another Value.

```
bool StrictEquals(const Value& other) const;
```

Returns a `bool` indicating if this Value strictly equals another Value.

```
Napi::Env Env() const;
```

Returns the `Env` environment this value is associated with.

```
bool IsEmpty() const;
```

Returns a `bool` indicating if this Value is *empty* (uninitialized).

An empty Value is invalid, and most attempts to perform an operation on an empty Value will result in an exception. Note an empty Value is distinct from JavaScript `null` or `undefined`, which are valid values.

When C++ exceptions are disabled at compile time, a method with a `Value` return type may return an empty Value to indicate a pending exception. So when not using C++ exceptions, callers should check whether this Value is empty before attempting to use it.

```
napi_valuetype Type() const;
```

Returns the `napi_valuetype` type of the Value.

```
bool IsUndefined() const;
```

Returns a `bool` indicating if this Value is an undefined JavaScript value.

```
bool IsNull() const;
```

Returns a `bool` indicating if this Value is a null JavaScript value.

```
bool IsBoolean() const;
```

Returns a `bool` indicating if this Value is a JavaScript boolean.

```
bool IsNumber() const;
```

Returns a `bool` indicating if this Value is a JavaScript number.

```
bool IsString() const;
```

Returns a `bool` indicating if this Value is a JavaScript string.

```
bool IsSymbol() const;
```

Returns a `bool` indicating if this Value is a JavaScript symbol.

```
bool IsArray() const;
```

Returns a `bool` indicating if this Value is a JavaScript array.

```
bool IsArrayBuffer() const;
```

Returns a `bool` indicating if this Value is a JavaScript array buffer.

```
bool IsTypedArray() const;
```

Returns a `bool` indicating if this Value is a JavaScript typed array.

```
bool IsObject() const;
```

Returns a `bool` indicating if this Value is JavaScript object.

```
bool IsFunction() const;
```

Returns a `bool` indicating if this Value is a JavaScript function.

```
bool IsBuffer() const;
```

Returns a `bool` indicating if this Value is a Node buffer.

```
template <typename T> T As() const;
```

Casts to another type of `Napi::Value`, when the actual type is known or assumed.

This conversion does not coerce the type. Calling any methods inappropriate for the actual value type will throw `Napi::Error`.

```
Boolean ToBoolean() const;
```

Returns the Value coerced to a JavaScript boolean.

```
Number ToNumber() const;
```
Returns the Value coerced to a JavaScript number.

```
String ToString() const;
```

Returns the Value coerced to a JavaScript string.

```
Object ToObject() const;
```

Returns the Value coerced to a JavaScript object.

## Callbackinfo

The object representing the components of the JavaScript request being made.

The Callbackinfo object is usually created and passed by the calling system.

The Callbackinfo object contains the arguments passed by the caller. The number of arguments is returned by the `Length` method. Each individual argument can be accessed using the `operator[]` method.

The `SetData` and `Data` methods are used to set and retrieve the data pointer contained in the Callbackinfo object. 

```
CallbackInfo(napi_env env, napi_callback_info info);
```

- `[in] env`: The `napi_env` environment in which to construct the `CallbackInfo` object.
- `[in] info`: The `napi_callback_info` data structure from which to construct the `CallbackInfo` object.

```
Napi::Env Env() const;
```

Returns the `Env` object in which the request is being made. 

```
Value NewTarget() const;
```

Returns the `new.target` value of the constructor call. If the CallbackInfo is not a constructor call, a call to `IsEmpty()` on the returned value returns true.

```
bool IsConstructCall() const;
```

Returns a `bool` indicating if the CallbackInfo is a constructor call. 
	
```
size_t Length() const;
```

Returns the number of arguemnts passed in the Callabckinfo object.

```
const Value operator [](size_t index) const;
```

- `[in] index`: The zero-based index of the requested argument.

Returns a `Value` object containing the requested argument. 

```
Value This() const;
```

Returns the JavaScript `this` value for the call

```
void* Data() const;
```

Returns the data pointer for the callback.

```
void SetData(void* data);
```

- `[in] data`: The new data pointer to associate with this Callbackinfo object.

### Not documented here 

```
~CallbackInfo();
// Disallow copying to prevent multiple free of _dynamicArgs
CallbackInfo(CallbackInfo const &) = delete;
void operator=(CallbackInfo const &) = delete;
```

## Reference (template)

Holds a counted reference to a Value; initially a weak reference unless otherwise specified, may be changed to/from a strong reference by adjusting the refcount.

The referenced Value is not immediately destroyed when the reference count is zero; it is merely then eligible for garbage-collection if there are no other references to the Value.

> I need to have a better understanding of Reference before I can complete this section.

## External (template)

The External template class implements the ability to create a Value object with arbitrary C++ data. It is the user's responsibility to manage the memory for the arbitrary C++ data.

External objects can be created with an optional Finalizer function and optional Hint value. The Finalizer function, if specified, is called when your External object is released by Node's garbage collector. It gives your code the opportunity to free any dynamically created data. If you specify a Hint value, it is passed to your Finalizer function. 

```
template <typename T>
static External New(napi_env env, T* data);
```

- `[in] env`: The `napi_env` environment in which to construct the External object.
- `[in] data`: The arbitrary C++ data to be held by the External object.

Returns the created `External<T>` object.

```
template <typename T>
static External New(napi_env env,
			T* data,
			Finalizer finalizeCallback);
```
- `[in] env`: The `napi_env` environment in which to construct the External object.
- `[in] data`: The arbitrary C++ data to be held by the External object.
- `[in] finalizeCallback`: A function called when the External object is released by the garbage collector accepting a T* and returning void.

Returns the created `External<T>` object.

```
template <typename T>
static External New(napi_env env,
			T* data,
			Finalizer finalizeCallback,
			Hint* finalizeHint);
```

- `[in] env`: The `napi_env` environment in which to construct the External object.
- `[in] data`: The arbitrary C++ data to be held by the External object.
- `[in] finalizeCallback`: A function called when the External object is released by the garbage collector accepting T* and Hint* parameters and returning void.
- `[in] finalizeHint`: A hint value passed to the `finalizeCallback` function.

Returns the created `External<T>` object.

```
T* Data() const;
```

Returns a pointer to the arbitrary C++ data held by the External object.

### Not documented here 

```
External();
External(napi_env env, napi_value value);
```
