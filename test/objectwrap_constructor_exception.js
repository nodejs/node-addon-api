'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const testUtil = require('./testUtil');

function test(binding) {
  return testUtil.runGCTests([
    'objectwrap constructor exception',
    () => {
      const { ConstructorExceptionTest } = binding.objectwrapConstructorException;
      assert.throws(() => (new ConstructorExceptionTest()), /an exception/);
    },
    // Do on gc before returning.
    () => {}
  ]);
}

module.exports = test(require(`./build/${buildType}/binding.node`))
  .then(() => test(require(`./build/${buildType}/binding_noexcept.node`)));
