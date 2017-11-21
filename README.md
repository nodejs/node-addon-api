# **Node.js API (N-API) Package**

This package contains **header-only C++ wrapper classes** for the 
**ABI-stable Node.js API** also known as **N-API**, providing C++ object 
model and exception handling semantics with low overhead. It guarantees 
backward compatibility when used with older versions of Node.js that do 
not have N-API built-in.

N-API is an API for building native addons. It is independent from the 
underlying JavaScript runtime (e.g. V8 or ChakraCore) and is maintained as 
part of Node.js itself. This API will be Application Binary Interface (ABI) 
stable across versions and flavors of Node.js. It is intended to insulate 
native addons from changes in the underlying JavaScript engine and allow 
modules compiled for one version to run on later versions of Node.js without 
recompilation. N-API guarantees the **API** and **ABI** compatibility across 
different versions of Node.js. So if you switched to a different version of 
Node.js, you would not need to reinstall or recompile the native addon.

APIs exposed by N-API are generally used to create and manipulate JavaScript
values. Concepts and operations generally map to ideas specified in the
**ECMA262 Language Specification**.

- **[Setup](#setup)**
- **[API Documentation](#api)**
- **[Examples](#examples)**
- **[Tests](#tests)**
- **[More resource and info about native Addons](#resources)**
- **[Code of Conduct](CODE_OF_CONDUCT.md)**
- **[Contributors](#contributors)**
- **[License](#license)**

## **Current version: 1.1.0**

(See [CHANGELOG.md](CHANGELOG.md) for complete Changelog)

[![NPM](https://nodei.co/npm/node-addon-api.png?downloads=true&downloadRank=true)](https://nodei.co/npm/node-addon-api/) [![NPM](https://nodei.co/npm-dl/node-addon-api.png?months=6&height=1)](https://nodei.co/npm/node-addon-api/)

<a name="setup"></a>

## Setup
  - [Installation and usage](doc/setup.md)
  - [node-gyp](doc/node-gyp.md)
  - [cmake-js](doc/cmake-js.md)
  - [Conversion tool](doc/conversion-tool.md)
  - [Generator](doc/generator.md)

<a name="api"></a>

### **API Documentation**
 - [Basic Types](doc/basic_types.md)
    - [Array](doc/array.md)
    - [Symbol](doc/symbol.md)
    - [String](doc/string.md)
    - [Name](doc/name.md)
    - [Number](doc/number.md)
    - [Boolean](doc/boolean.md)
    - [Env](doc/env.md)
    - [Value](doc/value.md)
    - [CallbackInfo](doc/callbackinfo.md)
    - [Reference](doc/reference.md)
    - [External](doc/external.md)
    - [Object](doc/object.md)
        - [ObjectReference](doc/object_reference.md)
        - [PropertyDescriptor](doc/property_descriptor.md)
 - [Error Handling](doc/error_handling.md)
    - [Error](doc/error.md)
 - [Object Lifettime Management](doc/object_lifetime_management.md)
    - [HandleScope](doc/handle_scope.md)
    - [EscapableHandleScope](doc/escapable_handle_scope.md)
 - [Working with JavaScript Values](doc/working_with_javascript_values.md)
    - [Function](doc/function.md)
        - [FunctionReference](doc/function_reference.md)
    - [ObjectWrap](doc/object_wrap.md)
        - [ClassPropertyDescriptor](doc/class_property_descriptor.md)
    - [Buffer](doc/buffer.md)
    - [ArrayBuffer](doc/array_buffer.md)
    - [TypedArray](doc/typed_array.md)
      - [TypedArrayOf](doc/typed_array_of.md)
 - [Async Operations](doc/async_operations.md)
    - [AsyncWorker](async_worker.md)
 - [Promises](doc/promises.md)

<a name="examples"></a>

### **Examples**

Are you new to **N-API**? Take a look at our **[examples](https://github.com/nodejs/abi-stable-node-addon-examples)**

- **[Hello World](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/1_hello_world/node-addon-api)**
- **[Pass arguments to a function](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/2_function_arguments/node-addon-api)**
- **[Callbacks](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/3_callbacks/node-addon-api)**
- **[Object factory](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/4_object_factory/node-addon-api)**
- **[Function factory](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/4_object_factory/node-addon-api)**
- **[Wrapping C++ Object](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/6_object_wrap/node-addon-api)**
- **[Factory of wrapped object](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/7_factory_wrap/node-addon-api)**
- **[Passing wrapped object around](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/8_passing_wrapped/node-addon-api)**

<a name="tests"></a>

### **Tests**

To run the **N-API** tests do:

```
npm install
npm test
```

Take a look and get inspired by our **[test suite](https://github.com/nodejs/node-addon-api/tree/master/test)**

<a name="resources"></a>

### **More resource and info about native Addons**
- **[C++ Addons](https://nodejs.org/dist/latest/docs/api/addons.html)**
- **[N-API](https://nodejs.org/dist/latest/docs/api/n-api.html)**
- **[N-API - Next Generation Node API for Native Modules](https://youtu.be/-Oniup60Afs)**

<a name="contributors"></a>

### WG Members / Collaborators
| Name                | GitHub link                                           |
| ------------------- | ----------------------------------------------------- |
| Anna Henningsen     | [addaleax](https://github.com/addaleax)               |
| Arunesh Chandra     | [aruneshchandra](https://github.com/aruneshchandra)   |
| Benjamin Byholm     | [kkoopa](https://github.com/kkoopa)                   |
| Gabriel Schulhof    | [gabrielschulhof](https://github.com/gabrielschulhof) |
| Hitesh Kanwathirtha | [digitalinfinity](https://github.com/digitalinfinity) |
| Jason Ginchereau    | [jasongin](https://github.com/jasongin)               |
| Michael Dawson      | [mhdawson](https://github.com/mhdawson)               |
| Sampson Gao         | [sampsongao](https://github.com/sampsongao)           |
| Taylor Woll         | [boingoing](https://github.com/boingoing)             |

<a name="license"></a>

Licensed under [MIT](./LICENSE.md)
