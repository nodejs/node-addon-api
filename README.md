# Node.js API (N-API) Package

This package contains **header-only C++ wrapper classes** for the **ABI-stable
Node.js API**  also known as **N-API**, providing C++ object model and exception
handling semantics with low overhead. It guarantees backward compatibility with 
use with older versions of Node.js that do not have N-API built-in.

Node.js API guarentees the **API** and **ABI** compatibility across different version of 
Node.js (**4** **5** **6** **7** and **8**). So if you switch to a different version of Node.js 
you must not reinstall and maybe recompile the Addon

N-API is an API for building native Addons. It is independent from the underlying
JavaScript runtime (ex V8) and is maintained as part of Node.js itself. This API 
will be Application Binary Interface (ABI) stable across versions of Node.js. It 
is intended to insulate Addons from changes in the underlying JavaScript engine 
and allow modules compiled for one version to run on later versions of Node.js 
without recompilation.

APIs exposed by N-API are generally used to create and manipulate JavaScript 
values. Concepts and operations generally map to ideas specified in the 
**ECMA262 Language Specification**.

## **Current version: 1.0.0**

(See [CHANHELOG.md](CHANGELOG.md) for complete Changelog)

[![NPM](https://nodei.co/npm/node-addon-api.png?downloads=true&downloadRank=true)](https://nodei.co/npm/dode-addon-api/) [![NPM](https://nodei.co/npm-dl/node-addon-api.png?months=6&height=1)](https://nodei.co/npm/dode-addon-api/)

## More resource and info about native Addons

### API Documentation

 - [Setup](doc/setup.md)
    - [node-gyp](doc/node-gyp.md)
    - [cmake-js](doc/cmake-js.md)
    - [Conversion tool](doc/conversion-tool.md)
    - [Generator](doc/generator.md)
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

### Getting Started

To use N-API in a native module:
  1. Add a dependency on this package to `package.json`:
```json
  "dependencies": {
    "node-addon-api": "1.0.0",
  }
```

  2. Reference this package's include directory and gyp file in `binding.gyp`:
```gyp
  'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
  'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
```

  3. Decide whether the package will enable C++ exceptions in the N-API wrapper.
     The base ABI-stable C APIs do not throw or handle C++ exceptions, but the
     N-API C++ wrapper classes may _optionally_
     [integrate C++ and JavaScript exception-handling
     ](https://nodejs.github.io/node-addon-api/class_napi_1_1_error.html).
     To enable that capability, C++ exceptions must be enabled in `binding.gyp`:
```gyp
  'cflags!': [ '-fno-exceptions' ],
  'cflags_cc!': [ '-fno-exceptions' ],
  'xcode_settings': {
    'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
    'CLANG_CXX_LIBRARY': 'libc++',
    'MACOSX_DEPLOYMENT_TARGET': '10.7',
  },
  'msvs_settings': {
    'VCCLCompilerTool': { 'ExceptionHandling': 1 },
  },
```
  Alternatively, disable use of C++ exceptions in N-API:
```gyp
  'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
```

  4. Include `napi.h` in the native module code.
     To ensure only ABI-stable APIs are used, DO NOT include
     `node.h`, `nan.h`, or `v8.h`.
```C++
#include "napi.h"
```

At build time, the N-API back-compat library code will be used only when the
targeted node version *does not* have N-API built-in.

## Conversion Tool
To make the migration to node-addon-api easier, we have provided a script to help
complete the steps listed above. To use the conversion script:
  1. Go to your module directory
```
cd [module_path]
```
  2. Install node-addon-api module
```
npm install node-addon-api
```
  3. Run node-addon-api conversion script
```
node ./node_modules/node-addon-api/tools/conversion.js ./
```
  4. While this script makes conversion easier, it still cannot fully convert
the module. The next step is to try to build the module and complete the
remaining conversions necessary to allow it to compile and pass all of the
module's tests.


<a name="collaborators"></a>
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
