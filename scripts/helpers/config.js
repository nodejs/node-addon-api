var version = require('../../package.json').version;

var sourceName = 'napi-implem'
var productExtension = 'node';

module.exports = {
  sourceName: sourceName,
  productExtension: productExtension,
  makeFileName: function(moduleVersion, extension) {
    return [sourceName, version, process.platform, process.arch]
      .concat(moduleVersion === undefined ? [] : [moduleVersion])
      .join('-') + '.' + (extension ? extension : productExtension);
  },
};
