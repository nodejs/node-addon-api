# Number

A Javascript number value.
## Methods

<b>Constructors</b>
</br>
Create a new Javascript Number from an int, float or double.
</br>
<i>Parameters</i>: [Env](),  ```double```
</br>
<i>Returns</i>:  ```Number``` 
```cpp
Number aNumberValue = Napi::Number::New(/*Environment*/ Env(), /*Value*/ 1.0);
```
Create a new empty Javascript Number.
</br>
<i>Parameters</i>:
</br>
<i>Returns</i>: ```Number``` 
```cpp
Number emptyNumber = Napi::Number();
```

<b>Extract a Number from [CallbackInfo]()</b>
</br>
Retrieve number from javascript to cpp
</br>
```cpp
Napi::Value
SomeClass::SomeMethod(const CallbackInfo &info)
{
    // to retrieve a number passed in as a parameter to SomeMethod
    // check that the type of value we want is in fact a Number
    if(info.Length() > 0 && info[0].IsNumber()) {
        double jsVal = info[0].As<Number>(); // returns type double as default
    }
}
```

<b>Casting</b>
</br>
Cast Javascript Number value to one of:
</br>
```int32_t```, ```uint32_t```, ```int64_t```, ```float```, ```double```
</br>
```cpp
uint32_t operatorVal = Number::New(Env(), 10.0); // Number to unsigned 32 bit integer
// or
auto instanceVal = info[0].As<Number>().Uint32Value();

assert(instanceVal ==  operatorVal)
```
