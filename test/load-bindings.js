'use strict';
module.exports = function loadBindings(name) {
  return require('../index')
    .loadModule(require('bindings')({
        bindings: name,
        path: true,
        module_root: __dirname
    }));
};
