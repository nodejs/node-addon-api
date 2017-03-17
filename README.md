# Node.js API (N-API) Package

This package contains header-only C++ wrapper classes for the ABI-stable
Node.js API (N-API).

To use these headers in a native module:
  1. Add a dependency on this package to `package.json`.
  2. Reference this package's include directory in `binding.gyp`:
```gyp
{
  'target_name': 'example_module',
  'include_dirs': ["<!(node -p \"require('node-api').include\")"],
}
```

Eventually this package will also contain library code that enables
backward-compatibility with use with older versions of Node.js that do
not have N-API built-in.

See the [main project README](
   https://github.com/nodejs/abi-stable-node/blob/doc/README.md)
for more details.
