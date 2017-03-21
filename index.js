var path = require('path');

// TODO: Check if the main node semantic version is within multiple ranges,
// or detect presence of built-in N-API by some other mechanism TBD.
var isNodeApiBuiltin = process.versions.modules >= 52;  // Node 8.x

var include = __dirname;
var gyp = path.join(__dirname, 'src', 'node_api.gyp');

if (isNodeApiBuiltin) {
   gyp += ':nothing';
} else {
   gyp += ':node-api';
   include = path.join(__dirname, 'src');
}

module.exports = {
   include: include,
   gyp: gyp,
   isNodeApiBuiltin: isNodeApiBuiltin,
};
