// Grab the prebuilds relevant to this platform and arch and store them in
// ./implems.

var https = require('https');
var pkg = require('../package.json');
var homepage = require('url').parse(pkg.binariesPage);
var path = require('path');
var tar = require('tar-stream');
var zlib = require('zlib');
var version = 'v' + pkg.version;
var fs = require('fs');
var implemDir = path.resolve(path.join(__dirname, '..', 'implems'));

// Request a prebuild for the given module version.
function requestOne(moduleVersion) {
  var options = {
    host: homepage.host,

    // We always join the paths with a "/" because this is for a URL
    path: [ homepage.pathname, [
        pkg.name,
        version,
        'node',
        moduleVersion,
        process.platform,
        process.arch
      ].join('-') + '.tar.gz'].join( '/' )
  };
  var extract = tar.extract();
  extract.on('entry', function(header, stream, next) {
    // The prebuild utility creates tarballs containing exactly one file, so we
    // expect that there will only be one "entry" event per request and that we
    // will thus not overwrite multiple times the file into which the
    // WriteStream empties.
    stream.pipe(fs.createWriteStream(
      path.join(implemDir, 'n-api-implem-' + moduleVersion + '.node')));
  });
  https.request(options, function(response) {
    if (response.statusCode === 200) {
      response.pipe(zlib.createGunzip()).pipe(extract);
    }
  }).end();
}

// Download all the prebuilds.
fs.mkdir(implemDir, function(error) {
  if (error && error.code != 'EEXIST') {
    throw error;
  }
  requestOne("v46");
  requestOne("v48");
  requestOne("v57");
});
