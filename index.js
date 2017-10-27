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
  (versionArray[0] > 8 || (versionArray[0] == 8 && versionArray[1] > 5));
var needsFlag = (!isNodeApiBuiltin && versionArray[0] == 8);

var include = path.join(__dirname, 'src');
var gyp = path.join(__dirname, 'src', 'node_api.gyp');

if (isNodeApiBuiltin || (process.platform !== 'win32' && !needsFlag)) {
   gyp += ':nothing';
} else {
   gyp += ':node-api';
   include = path.join(__dirname, 'external-napi');
}

function loadModule(filename) {
  var result;
  if (!isNodeApiBuiltin && process.platform !== 'win32') {

    // On non-Windows node versions wheren N-API is external we tell
    // n-api-loader what implementation and what module to load via environment
    // variables.
    process.env.NAPI_MODULE_FILE_NAME = filename;
    process.env.NAPI_IMPLEM_FILE_NAME =
      path.join(__dirname, 'implems',
        'n-api-implem-v' + process.versions.modules + '.node');

    // We try loading n-api-loader from both Release and Debug.
    try {
      result = require('./build/Release/n-api-loader.node');
    } catch (anError) {
      try {
        result = require('./build/Debug/n-api-loader.node');
      } catch (anError) {}
    }

    // We remove the environment variables we added.
    delete process.env.NAPI_MODULE_FILE_NAME;
    delete process.env.NAPI_IMPLEM_FILE_NAME;
  }

  // Finally, if loading the module via n-api-loader failed or we haven't even
  // tried because we're on win32 or N-API is built-in, we load the module the
  // conventional way.
  return result || module.parent.require(filename);
}

module.exports = {
   include: [ '"' + include + '"', '"' + __dirname + '"' ].join(' '),
   gyp: gyp,
   isNodeApiBuiltin: isNodeApiBuiltin,
   needsFlag: needsFlag,
   loadModule: loadModule
};
