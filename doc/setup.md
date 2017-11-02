## Setup

To use N-API in a native module:

  1. Add a dependency on this package to `package.json`:

      ```json
        "dependencies": {
        "node-addon-api": "1.0.0",
        }
      ```

  1. Reference this package's include directory, libraries, and linker flags in
     `binding.gyp`:

      ```gyp
        'include_dirs': ["<!@(node -p \"require('../').include\")"],
        'libraries': ["<!@(node -p \"require('../').libraries\")"],
        'ldflags': ["<!@(node -p \"require('../').ldflags\")"],
        'xcode_settings': {
          'OTHER_LDFLAGS': ["<!@(node -p \"require('../').ldflags\")"],
        }
      ```

  1. Decide whether the package will enable C++ exceptions in the N-API wrapper.
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

  1. Include `napi.h` in the native module code.
     To ensure only ABI-stable APIs are used, DO NOT include
     `node.h`, `nan.h`, or `v8.h`.

      ```C++
      #include "napi.h"
      ```

  1. As long as you wish to support Node.js versions that do not have N-API
     built-in, load your native module with

     ```js
     require('node-addon-api').loadModule(pathToMyModule);
     ```

     When all versions of Node.js in your supported version range have N-API
     built-in, you can return to loading your native module with

     ```js
     require(pathToMyModule);
     ```

At build time, the N-API back-compat library code will be used only when the
targeted node version *does not* have N-API built-in.
