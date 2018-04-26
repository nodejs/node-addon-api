## Object Wrap

The ```ObjectWrap``` class can be used to expose C++ code to JavaScript. To do
this you need to extend the ObjectWrap class that contain all the plumbing to connect
JavaScript code to a C++ object.
Classes extending ```ObjectWrap``` can be instantiated from JavaScript using the
**new** operator, and their methods can be directly invoked from JavaScript.
The **wrap** word refers to a way to group methods and state of your class because it
will be your responsibility write custom code to bridge each of your C++ class methods.

You are reading a draft of the next documentation and it's in continuous update so
if you don't find what you need please refer to:
[C++ wrapper classes for the ABI-stable C APIs for Node.js](https://nodejs.github.io/node-addon-api/)
