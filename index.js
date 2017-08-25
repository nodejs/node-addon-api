var path = require('path');

var versionArray = process.version
  .substr(1)
  .replace(/-.*$/, '')
  .split('.')
  .map(function(item) {
    return +item;
  });

// TODO: Check if the main node semantic version is within multiple ranges,
// or detect presence of built-in N-API by some other mechanism TBD.

// We know which version of Node.js first shipped the incarnation of the API
// available in *this* package. So, if we find that the Node.js version is below
// that, we indicate that the API is missing from Node.js.
var isNodeApiBuiltin =
  (versionArray[0] >= 8 && versionArray[1] >= 4 && versionArray[2] >= 0);

// So far it looks like even version 9 will need the flag. We need to adjust
// this for the version where the flag is dropped whenever that version lands.
var needsFlag = (versionArray[0] >= 8);

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
   needsFlag: needsFlag
};
