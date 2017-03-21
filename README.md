# Node.js API (N-API) Package

This package contains header-only C++ wrapper classes for the ABI-stable
Node.js API (N-API), along with library code that enables
backward-compatibility with use with older versions of Node.js that do
not have N-API built-in.

To use N-API in a native module:
  1. Add a dependency on this package to `package.json`. (It is not yet published to npm, so reference it directly from GitHub.)
```json
  "dependencies": {
    "node-api": "github:nodejs/node-api",
  }
```

  2. Reference this package's include directory and gyp file in `binding.gyp`:
```gyp
{
  'target_name': 'example_module',
  'include_dirs': ["<!(node -p \"require('node-api').include\")"],
  'dependencies': ["<!(node -p \"require('node-api').gyp\")"],
}
```

  3. Include `napi.h` in the native module code:
```C++
#include "napi.h"
```

At build time, the N-API back-compat library code will be used only when the targeted node version *does not* have N-API built-in.

See the [main project README](
   https://github.com/nodejs/abi-stable-node/blob/doc/README.md)
for more details about N-API.
