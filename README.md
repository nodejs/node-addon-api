# **node-addon-api module** 
This module contains **header-only C++ wrapper classes** which simplify
the use of the C based [N-API](https://nodejs.org/dist/latest/docs/api/n-api.html)
provided by Node.js when using C++. It provides a C++ object model
and exception handling semantics with low overhead.

N-API is an ABI stale API for building native addons in C++. It is
independent from the underlying JavaScript runtime (e.g. V8 or ChakraCore)
and is maintained as part of Node.js itself. It is intended to insulate 
native addons from changes in the underlying JavaScript engine and allow 
modules compiled for one version to run on later versions of Node.js without 
recompilation.

The node-addon-api module preserves the benefits of the N-API as it consists
only of inline code that depends only on the stable API provided by N-API.
As such, modules built against one version of Node.js using node-addon-api
should run without having to be rebuilt with newer versions of Node.js.
As new APIs are added to N-API, node-addon-api must be updated to provide
wrappers for those new APIs. For this reason node-addon-api provides
methods that allow callers to obtain the underlying N-API handles so
direct calls to N-API and the use of the objects/methods provided by
node-addon-api can be used together. For example, in order to be able
to use an API for which the node-add-api does not yet provide a wrapper.

APIs exposed by node-addon-api are generally used to create and
manipulate JavaScript values. Concepts and operations generally map
to ideas specified in the **ECMA262 Language Specification**.

- **[Setup](#setup)**
- **[API Documentation](#api)**
- **[Examples](#examples)**
- **[Tests](#tests)**
- **[More resource and info about native Addons](#resources)**
- **[Code of Conduct](CODE_OF_CONDUCT.md)**
- **[Contributors](#contributors)**
- **[License](#license)**

## **Current version: 1.3**

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

The following is the documentation for node-addon-api (NOTE:
still a work in progress as its not yet complete).

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
    - [TypeError](doc/type_error.md)
    - [RangeError](doc/range_error.md)
 - [Object Lifetime Management](doc/object_lifetime_management.md)
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
    - [AsyncWorker](doc/async_worker.md)
 - [Promises](doc/promises.md)

<a name="examples"></a>

### **Examples**

Are you new to **node-addon-api**? Take a look at our **[examples](https://github.com/nodejs/abi-stable-node-addon-examples)**

- **[Hello World](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/1_hello_world/node-addon-api)**
- **[Pass arguments to a function](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/2_function_arguments/node-addon-api)**
- **[Callbacks](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/3_callbacks/node-addon-api)**
- **[Object factory](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/4_object_factory/node-addon-api)**
- **[Function factory](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/5_function_factory/node-addon-api)**
- **[Wrapping C++ Object](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/6_object_wrap/node-addon-api)**
- **[Factory of wrapped object](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/7_factory_wrap/node-addon-api)**
- **[Passing wrapped object around](https://github.com/nodejs/abi-stable-node-addon-examples/tree/master/8_passing_wrapped/node-addon-api)**

<a name="tests"></a>

### **Tests**

To run the **node-addon-api** tests do:

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
| Nicola Del Gobbo    | [NickNaso](https://github.com/NickNaso)               |
| Sampson Gao         | [sampsongao](https://github.com/sampsongao)           |
| Taylor Woll         | [boingoing](https://github.com/boingoing)             |

<a name="license"></a>

Licensed under [MIT](./LICENSE.md)
