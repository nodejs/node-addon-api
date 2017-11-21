# Number

A Javascript number value.
## Methods


### Constructor

```cpp
Napi::Number::New(Napi::Env env, double value);
```
 - `[in] env`: The `napi_env` Environment
 - `[in] value`: The value the Javascript Number will contain

```cpp
Napi::Number();
```
returns a new empty Javascript Number

You can easily cast a Javascript number to one of:
 - int32_t
 - uint32_t
 - int64_t
 - float
 - double

The following shows an example of casting a number to an uint32_t value.

```cpp
uint32_t operatorVal = Number::New(Env(), 10.0); // Number to unsigned 32 bit integer
// or
auto instanceVal = info[0].As<Number>().Uint32Value();
```
