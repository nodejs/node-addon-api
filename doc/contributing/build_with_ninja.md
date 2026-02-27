# Build Test with Ninja

Ninja can be used to speed up building tests with optimized parallelism.

To build the tests with ninja and node-gyp, run the following commands:

```sh
/node-addon-api $ node-gyp configure -C test -- -f ninja
/node-addon-api $ ninja -C test/build/Release
# Run tests
/node-addon-api $ node ./test/index.js

# Run tests with debug addon
/node-addon-api $ ninja -C test/build/Debug
/node-addon-api $ NODE_API_BUILD_CONFIG=Debug node ./test/index.js
```
