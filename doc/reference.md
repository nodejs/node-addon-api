You are reading a draft of the next documentation and it's in continuos update so
if you don't find what you need please refer to:
[C++ wrapper classes for the ABI-stable C APIs for Node.js](https://nodejs.github.io/node-addon-api/)

# Reference (template)

Holds a counted reference to a [Value](value.md) object; initially a weak reference unless otherwise specified, may be changed to/from a strong reference by adjusting the refcount.

The referenced Value is not immediately destroyed when the reference count is zero; it is merely then eligible for garbage-collection if there are no other references to the Value.

Reference objects allocated in static space, such as a global static instance, must call the `SuppressDestruct` method to prevent its destructor, running at program shutdown time, from attempting to reset the reference when the environment is no longer valid.

The following classes inherit, either directly or indirectly, from Reference:

* [ObjectWrap](object_wrap.md)
* [ObjectReference](object_reference.md)
* [FunctionReference](function_reference.md)

## Methods

### Factory Method

```cpp
static Reference<T> New(const T& value, uint32_t initialRefcount = 0);
```

* `[in] value`: The value which is to be referenced.

* `[in] initialRefcount`: The initial reference count.

### Empty Constructor

```cpp
Reference();
```

Creates a new _empty_ Reference instance.

### Constructor

```cpp
Reference(napi_env env, napi_value value);
```

* `[in] env`: The `napi_env` environment in which to construct the Reference object.

* `[in] value`: The N-API primitive value to be held by the Reference.

### Env

```cpp
Napi::Env Env() const;
```

Returns the `Env` value in which the Reference was instantiated.

### IsEmpty

```cpp
bool IsEmpty() const;
```

Determines whether the value held by the Reference is empty.

### Value

```cpp
T Value() const;
```

Returns the value held by the Reference.

### Ref

```cpp
uint32_t Ref();
```

Increments the reference count for the Reference and returns the resulting reference count. Throws an error if the increment fails.

### Unref

```cpp
uint32_t Unref();
```

Decrements the reference count for the Reference and returns the resulting reference count. Throws an error if the decrement fails.

### Reset (Empty)

```cpp
void Reset();
```

Sets the value held by the Reference to be empty.

### Reset

```cpp
void Reset(const T& value, uint32_t refcount = 0);
```

* `[in] value`: The value which is to be referenced.

* `[in] initialRefcount`: The initial reference count.

Sets the value held by the Reference.

### SuppressDestruct

```cpp
void SuppressDestruct();
```

Call this method on a Reference that is declared as static data to prevent its destructor, running at program shutdown time, from attempting to reset the reference when the environment is no longer valid.
