# Node.js API (N-API) Package

This package contains header-only C++ wrapper classes for the ABI-stable
Node.js API (N-API), along with library code that enables
backward-compatibility with use with older versions of Node.js that do
not have N-API built-in.

To use N-API in a native module:
  1. Add a dependency on this package to `package.json`. 
  It is not yet published to npm, so reference it directly from GitHub.
```json
  "dependencies": {
    "node-api": "github:nodejs/node-api",
  }
```

  2. Reference this package's include directory and gyp file in `binding.gyp`:
```gyp
  'include_dirs': ["<!(node -p \"require('node-api').include\")"],
  'dependencies': ["<!(node -p \"require('node-api').gyp\")"],
```

  3. Ensure C++ exceptions are enabled in `binding.gyp`.
     The N-API C++ wrapper classes use exceptions for error-handling;
     the base ABI-stable C APIs do not.
```gyp
  'cflags!': [ '-fno-exceptions' ],
  'cflags_cc!': [ '-fno-exceptions' ],
  'xcode_settings': { 'GCC_ENABLE_CPP_EXCEPTIONS': 'YES' },
```

  4. Include `napi.h` in the native module code.
     To ensure only ABI-stable APIs are used, DO NOT include `node.h` or `v8.h`.
```C++
#include "napi.h"
```

At build time, the N-API back-compat library code will be used only when the targeted node version *does not* have N-API built-in.

See the [main project README](
   https://github.com/nodejs/abi-stable-node/blob/doc/README.md)
for more details about N-API.
