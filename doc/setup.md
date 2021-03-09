# Setup

## Prerequisites

Before starting to use **Node-API** you need to assure you have the following
prerequisites:

* **Node.JS** see: [Installing Node.js](https://nodejs.org/)

* **Node.js native addon build tool**

  - **[node-gyp](node-gyp.md)**

## Installation and usage

To use **Node-API** in a native module:

  1. Add a dependency on this package to `package.json`:

```json
  "dependencies": {
    "node-addon-api": "*",
  }
```

  2. Reference this package's include directory and gyp file in `binding.gyp`:

```gyp
  'include_dirs': ["<!(node -p \"require('node-addon-api').include_dir\")"],
```

  3. Decide whether the package will enable C++ exceptions in the Node-API wrapper.
     The base ABI-stable C APIs do not throw or handle C++ exceptions, but the
     Node-API C++ wrapper classes may _optionally_
     [integrate C++ and JavaScript exception-handling
     ](https://github.com/nodejs/node-addon-api/blob/HEAD/doc/error_handling.md).
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

  Alternatively, disable use of C++ exceptions in Node-API:

```gyp
  'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
```

  If you decide to use node-addon-api without C++ exceptions enabled, please
  consider enabling node-addon-api safe API type guards to ensure proper
  exception handling pattern:

```gyp
  'defines': [ 'NODE_ADDON_API_ENABLE_MAYBE' ],
```

  4. If you would like your native addon to support OSX, please also add the
  following settings in the `binding.gyp` file:

  ```gyp
  'conditions': [
    ['OS=="mac"', {
        'cflags+': ['-fvisibility=hidden'],
        'xcode_settings': {
          'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES', # -fvisibility=hidden
        }
    }]
  ]
  ```

  5. Include `napi.h` in the native module code.
     To ensure only ABI-stable APIs are used, DO NOT include
     `node.h`, `nan.h`, or `v8.h`.

```C++
#include "napi.h"
```

At build time, the Node-API back-compat library code will be used only when the
targeted node version *does not* have Node-API built-in.

The preprocessor directive `NODE_ADDON_API_DISABLE_DEPRECATED` can be defined at
compile time before including `napi.h` to skip the definition of deprecated APIs.
