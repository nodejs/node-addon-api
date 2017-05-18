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
    "node-addon-api": "0.2.0",
  }
```

  2. Reference this package's include directory and gyp file in `binding.gyp`:
```gyp
  'include_dirs': ["<!(node -p \"require('node-addon-api').include\")"],
  'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
```

  3. Ensure C++ exceptions are enabled in `binding.gyp`.
     The N-API C++ wrapper classes use exceptions for error-handling;
     the base ABI-stable C APIs do not.
```gyp
  'cflags!': [ '-fno-exceptions' ],
  'cflags_cc!': [ '-fno-exceptions' ],
  'xcode_settings': { 'GCC_ENABLE_CPP_EXCEPTIONS': 'YES' },
  'msvs_settings': {
    'VCCLCompilerTool': { 'ExceptionHandling': 1 },
  },
```

  4. Include `napi.h` in the native module code.
     To ensure only ABI-stable APIs are used, DO NOT include
     `node.h`, `nan.h`, or `v8.h`.
```C++
#include "napi.h"
```

At build time, the N-API back-compat library code will be used only when the
targeted node version *does not* have N-API built-in.

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
