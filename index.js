const path = require('path');

const include = path.relative('.', __dirname);

module.exports = {
  include: include,
  gyp: path.join(include, 'node_api.gyp:nothing'),
  isNodeApiBuiltin: true,
  needsFlag: false
};
