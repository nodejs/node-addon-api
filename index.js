const path = require('path');

module.exports = {
  include: `"${__dirname}"`,
  gyp: path.join(__dirname, 'node_api.gyp:nothing'),
  isNodeApiBuiltin: true,
  needsFlag: false
};
