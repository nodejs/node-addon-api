# Object Reference

ObjectReference is a subclass of [Reference](reference.md), and is equivalent to an instance of `Reference<Object>`. This means that an ObjectReference holds an [Object](object.md), and a count of the number of references to that Object. When the count is greater than 0, an ObjectReference is not eligible for garbage collection. This ensures that the Object being held as a value of the ObjectReference will remain accessible, even if the original Object no longer is. However, ObjectReference is unique from a Reference since properties can be set and get to the Object itself that can be accessed through the ObjectReference.

For more general information on references, please consult [Reference](referenc.md).

## Example
```cpp
#include <napi.h>

using namescape Napi;

void Init(Env env) {

    // Create an empty ObjectReference that has an initial reference count of 2.
    ObjectReference obj_ref = Reference<Object>::New(Object::New(env), 2);

    // Set a couple of different properties on the reference.
    obj_ref.Set("hello", String::New(env, "world"));
    obj_ref.Set(42, "The Answer to Life, the Universe, and Everything");

    // Get the properties using the keys.
    Value val1 = obj_ref.Get("hello");
    Value val2 = obj_ref.Get(42);
}
```

## Methods

### Initialization

```cpp
static ObjectReference New(const Object& value, uint32_t initialRefcount = 0);
```

* `[in] value`: The Object which is to be referenced.

* `[in] initialRefcount`: The initial reference count.

Returns the newly created reference.

```cpp
static ObjectReference Weak(const Object& value);
```

Creates a "weak" reference to the value, in that the initial count of number of references is set to 0.

* `[in] value`: The value which is to be referenced.

Returns the newly created reference.

```cpp
static ObjectReference Persistent(const Object& value);
```

Creates a "persistent" reference to the value, in that the initial count of number of references is set to 1.

* `[in] value`: The value which is to be referenced.

Returns the newly created reference.

### Empty Constructor

```cpp
ObjectReference();
```

Returns a new _empty_ ObjectReference instance.

### Constructor

```cpp
ObjectReference(napi_env env, napi_value value);
```

* `[in] env`: The `napi_env` environment in which to construct the ObjectReference object.

* `[in] value`: The N-API primitive value to be held by the ObjectReference.

Returns the newly created reference.

### Set
```cpp
void Set(___ key, ___ value);
```

* `[in] key`: The name for the property being assigned.

* `[in] value`: The value being assigned to the property.

The `key` can be any of the following types:
- `const char*`
- `const std::string`
- `uint32_t`

The `value` can be any of the following types:
- `napi_value`
- `Napi::Value`
- `const char*`
- `bool`
- `double`

### Get

```cpp
Value Get(___ key);
```

* `[in] key`: The name of the property to return the value for.

Returns the [Value](value.md) associated with the key property. Returns NULL if no such key exists.

The `key` can be any of the following types:
- `const char*`
- `const std::string`
- `uint32_t`

