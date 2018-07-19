# Property Descriptor

An [Object](object.md) can be assigned properites via its [DefineProperty](object.md#defineproperty) and [DefineProperties](object.md#defineproperties) function, which take PropertyDescrptor(s) as their parameters. The PropertyDescriptor can contain either values or functions, which are then assigned to the Object. Note that a single instance of a PropertyDescriptor class can only contain either one value, or at most two functions. PropertyDescriptors can only be created through the class methods [Accessor](#accessor), [Function](#function), or [Value](#value), each of which return a new static instance of a PropertyDescriptor.

## Example

```cpp
#include <napi.h>

using namespace Napi;

Value TestGetter(const CallbackInfo& info) {
   return Boolean::New(info.Env(), testValue);
}

void TestSetter(const CallbackInfo& info) {
   testValue = info[0].As<Boolean>();
}

Value TestFunction(const CallbackInfo& info) {
   return Boolean::New(info.Env(), true);
}

Void Init(Env env) {
   // Accessor
   PropertyDescriptor pd1 = PropertyDescriptor::Accessor("pd1", TestGetter);
   PropertyDescriptor pd2 = PropertyDescriptor::Accessor("pd2", TestGetter, TestSetter);

   // Function
   PropertyDescriptor pd3 = PropertyDescriptor::Function("function", TestFunction);

   // Value
   Boolean true_bool = Boolean::New(env, true);
   PropertyDescriptor pd4 = PropertyDescriptor::Value("boolean value", TestFunction, napi_writable);

  // Assign to an Object
  Object obj = Object::New(env);
  obj.DefineProperties({pd1, pd2, pd3, pd4});
}
```

## Methods

### Constructor

```cpp
Napi::PropertyDescriptor::PropertyDescriptor (napi_property_descriptor desc);
```

* `[in] desc`: A PropertyDescriptor that is needed in order to create another PropertyDescriptor.

### Accessor

```cpp
static PropertyDescriptor Napi::PropertyDescriptor::Accessor (___ name,
                Getter getter,
                napi_property_attributes attributes = napi_default,
                void *data = nullptr);
```

* `[in] name`: The name used for the getter function.
* `[in] getter`: A getter function.
* `[in] attributes`: Potential attributes for the getter function.
* `[in] data`: A pointer to data of any type, default is a null pointer.

Returns a PropertyDescriptor that contains a function.

The name of the property can be any of the following types:
- `const char*`
- `const std::string &`
- `napi_value value`
- `Name`

```cpp
static PropertyDescriptor Napi::PropertyDescriptor::Accessor (___ name,
                Getter getter,
                Setter setter,
                napi_property_attributes attributes = napi_default,
                void *data = nullptr);
```

* `[in] name`: The name of the getter and setter function.
* `[in] getter`: The getter function.
* `[in] setter`: The setter function.
* `[in] attributes`: Potential attributes for the getter function.
* `[in] data`: A pointer to data of any type, default is a null pointer.

Returns a PropertyDescriptor that contains a Getter and Setter function.

The name of the property can be any of the following types:
- `const char*`
- `const std::string &`
- `napi_value value`
- `Name`

### Function

```cpp
static PropertyDescriptor Napi::PropertyDescriptor::Function (___ name,
                Callable cb,
                napi_property_attributes attributes = napi_default,
		            void *data = nullptr);
```

* `[in] name`: The name of the Callable function.
* `[in] cb`: The function
* `[in] attributes`: Potential attributes for the getter function.
* `[in] data`: A pointer to data of any type, default is a null pointer.

Returns a PropertyDescriptor that contains a callable Function.

The name of the property can be any of the following types:
- `const char*`
- `const std::string &`
- `napi_value value`
- `Name`

### Value

```cpp
static PropertyDescriptor Napi::PropertyDescriptor::Value (___ name,
                napi_value value,
                napi_property_attributes attributes = napi_default);
```

The name of the property can be any of the following types:
- `const char*`
- `const std::string &`
- `napi_value value`
- `Name`

## Related Information

### napi\_property\_attributes
`napi_property_attributes` are flags used to indicate to JavaScript certain permissions that the property is meant to have. The following are the flag options:
- napi\_default,
- napi\_writable,
- napi\_enumerable,
- napi\_configurable
For more information on the flags and on napi\_property\_attributes, please read the documentation [here](https://github.com/nodejs/node/blob/master/doc/api/n-api.md#napi_property_attributes).

