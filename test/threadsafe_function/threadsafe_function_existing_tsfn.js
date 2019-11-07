'use strict';

const assert = require('assert');

const buildType = process.config.target_defaults.default_configuration;

module.exports = Promise.all([
  test(require(`../build/${buildType}/binding.node`)),
  test(require(`../build/${buildType}/binding_noexcept.node`))
]);

async function test(binding) {
  const testCall = binding.threadsafe_function_existing_tsfn.testCall;
  
  assert(typeof await testCall({ blocking: true,  data: true  }) === "number");
  assert(typeof await testCall({ blocking: true,  data: false }) === "undefined");
  assert(typeof await testCall({ blocking: false, data: true  }) === "number");
  assert(typeof await testCall({ blocking: false, data: false }) === "undefined");
}
