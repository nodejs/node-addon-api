var path = require('path');

// We know which version of Node.js first shipped the incarnation of the API
// available in *this* package. So, if we find that the Node.js version is below
// that, we indicate that the API is missing from Node.js.
function getNodeApiBuiltin() {
  var versionArray = process.version
    .substr(1)
    .split('.')
    .map(function(item) {
      return +item;
    });
  return versionArray[0] >= 8 && versionArray[1] >= 4 && versionArray[2] >= 0;
}

// TODO: Check if the main node semantic version is within multiple ranges,
// or detect presence of built-in N-API by some other mechanism TBD.
var isNodeApiBuiltin = getNodeApiBuiltin();

var include = path.join(__dirname, 'src');
var gyp = path.join(__dirname, 'src', 'node_api.gyp');

if (isNodeApiBuiltin) {
   gyp += ':nothing';
} else {
   gyp += ':node-api';
   include = path.join(__dirname, 'external-napi');
}

module.exports = {
   include: [ '"' + include + '"', '"' + __dirname + '"' ].join(' '),
   gyp: gyp,
   isNodeApiBuiltin: isNodeApiBuiltin,
};
