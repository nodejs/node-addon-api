# Class propertry and descriptor

Property descriptor for use with `Napi::ObjectWrap::DefineClass()`.
This is different from the standalone `Napi::PropertyDescriptor` because it is
specific to each `Napi::ObjectWrap<T>` subclass.
This prevents using descriptors from a different class when defining a new class
(preventing the callbacks from having incorrect `this` pointers).

## Methods

### Constructor

Creates new instance of `Napi::ClassPropertyDescriptor` descriptor object.

```cpp
Napi::ClassPropertyDescriptor(napi_property_descriptor desc) : _desc(desc) {}
```

- `[in] desc`: The `napi_property_descriptor`

Returns new instance of `Napi::ClassPropertyDescriptor` that is used as property descriptor
inside the `Napi::ObjectWrap<T>` class.

### Operator

```cpp
operator napi_property_descriptor&() { return _desc; }
```

Returns the original N-API `napi_property_descriptor` wrapped inside the `Napi::ClassPropertyDescriptor`

```cpp
operator const napi_property_descriptor&() const { return _desc; }
```

Returns the original N-API `napi_property_descriptor` wrapped inside the `Napi::ClassPropertyDescriptor`
