var path = require('path');
var productDir = require('./product-dir.json').productDir;
var config = require('./scripts/helpers/config');
var shelljs = require('shelljs');

var versionArray = process.version
  .substr(1)
  .replace(/-.*$/, '')
  .split('.')
  .map(function(item) {
    return +item;
  });

// We know which version of Node.js first shipped the incarnation of the API
// available in *this* package. So, if we find that the Node.js version is below
// that, we indicate that the API is missing from Node.js.
var isNodeApiBuiltin =
  (versionArray[0] > 8 || (versionArray[0] == 8 && versionArray[1] > 5));

var implemsDir = path.join(__dirname, 'implems');

function loadModule(filename) {
  var result;
  var implemDestination = path.join(
    (process.platform === 'win32' ? path.dirname(filename) :
      implemsDir), 'napi-implem.' + config.productExtension);
  if (!(filename in require.cache)) {

    // On Linux we need to remove the file before copying it, because copying
    // replaces the contents of the original file, and the dynamic loader will
    // get confused and segfault.
    if (process.platform === 'linux') {
      shelljs.rm('-f', implemDestination);
    }
    shelljs.cp(
      path.join(implemsDir, config.makeFileName(process.versions.modules)),
      implemDestination);

    // Linux doesn't load modules with RTLD_DEEPBIND by default, so we need to
    // load the addon through an implementation-agnostic loader DSO. We pass it
    // the file name of the addon we wish to load via an environment variable.
    if (process.platform === 'linux') {
      process.env.NAPI_MODULE_FILENAME = filename;
      result = require(path.join(productDir, 'n-api-loader.node'));
      delete process.env.NAPI_MODULE_FILENAME;
      return result;
    }
  }
  return module.parent.require(filename);
}

module.exports = {
  include: [
    '"' + path.join(__dirname, 'src') + '"',
    '"' + __dirname + '"'
  ].join(' '),
  isNodeApiBuiltin: isNodeApiBuiltin,
  loadModule: loadModule,

  // Link to the version of the library that does not specify an ABI
  libraries: isNodeApiBuiltin ? '' :
    '"' + path.join(implemsDir, config.sourceName + '.' +
      (process.platform === 'win32' ? 'lib' : config.productExtension)) + '"',
  ldflags: isNodeApiBuiltin ? '' :
    (process.platform === 'aix' ? '-Wl,-blibpath:' + implemsDir :
      [ '-Wl,-rpath', implemsDir ].join(' '))
};
