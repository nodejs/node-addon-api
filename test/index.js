'use strict';
global.buildType = process.config.target_defaults.default_configuration;
global.binding = require(`./build/${buildType}/binding.node`);
global.assert = require('assert');

let testModules = [
   'error',
   'function',
];

testModules.forEach(name => {
   try {
      require('./' + name);
   }
   catch (e) {
      console.error(e);
   }
});
