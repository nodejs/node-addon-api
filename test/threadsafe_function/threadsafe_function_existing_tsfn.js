'use strict';

const assert = require('assert');

const buildType = process.config.target_defaults.default_configuration;

module.exports = test(require(`../build/${buildType}/binding.node`))
  .then(() => test(require(`../build/${buildType}/binding_noexcept.node`)));

async function test(binding) {
  const testCall = binding.threadsafe_function_existing_tsfn.testCall;
  
  assert.strictEqual(typeof await testCall({ blocking: true,  data: true  }), "number");
  assert.strictEqual(typeof await testCall({ blocking: true,  data: false }), "undefined");
  assert.strictEqual(typeof await testCall({ blocking: false, data: true  }), "number");
  assert.strictEqual(typeof await testCall({ blocking: false, data: false }), "undefined");
}
