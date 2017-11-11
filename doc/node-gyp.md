# node-gyp

You are reading a draft of the next documentation and it's in continuos update so
if you don't find what you need please refer to: 
[C++ wrapper classes for the ABI-stable C APIs for Node.js](https://nodejs.github.io/node-addon-api/)

C++ code needs to be compiled into executable form whether it be as an object
file to linked with others, a shared library, or a standalone executable. The 
reasons for this is that we need to link to the Node.js dependencies and headers
correcrtly and when we publish our modules we wiil need a cross platform way to
build our C++ source into binary for the target platform.

**node-gyp** is the de-fafto standard build tool for writing Node.js addons. It's
base on Google's **gyp** build tool, which abstract away many of the tedious 
issues related to cross platform building.

