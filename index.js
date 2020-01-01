const path = require('path');

module.exports = {
  include: `"${__dirname}"`,
  gyp: path.join(__dirname, 'nothing.gyp:nothing'),
  isNodeApiBuiltin: true,
  needsFlag: false
};
