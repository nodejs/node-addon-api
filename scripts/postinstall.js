// Grab the binaries relevant to this platform and arch and store them in
// ./implems.

var fs = require('fs');
var path = require('path');
var https = require('https');
var downloadPage =
  require('url').parse(require('../package.json').binariesPage);
var implemDir = path.resolve(path.join(__dirname, '..', 'implems'));
var config = require('./helpers/config');
var tar = require('tar-stream');
var zlib = require('zlib');

// Request a prebuild for the given module version.
function requestOne(moduleVersion) {
  https.request({
    host: downloadPage.host,
    path: [
      downloadPage.pathname,
      config.makeFileName(moduleVersion, 'tgz')
    ].join('/')
  }, function responseHandler(response) {
    var extract = tar.extract();
    extract.on('entry', function(header, stream, next) {
      stream.pipe(fs.createWriteStream(path.join(implemDir, header.name)));
      stream.on('end', next);
    });
    if (response.statusCode === 200) {
      response.pipe(new zlib.createGunzip()).pipe(extract);
    }
  }).end();
}

// Download all the prebuilds.
fs.mkdir(implemDir, function(error) {
  if (error && error.code != 'EEXIST') {
    throw error;
  }
  requestOne("46");
  requestOne("48");
  requestOne("57");
  requestOne("57b");
});
