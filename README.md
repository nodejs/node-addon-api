# Node.js API (N-API) Package

This package contains header-only C++ wrapper classes for the ABI-stable
Node.js API (N-API), along with library code that enables
backward-compatibility with use with older versions of Node.js that do
not have N-API built-in.

### API Documentation

 - [ABI-Stable C APIs in Node.js](https://nodejs.org/api/n-api.html)
 - [C++ APIs in this package](https://nodejs.github.io/node-addon-api/namespace_napi.html)

### Getting Started

To use N-API in a native module:
  1. Add a dependency on this package to `package.json`:
```json
  "dependencies": {
    "node-addon-api": "0.3.3",
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
