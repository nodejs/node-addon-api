'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

const test = (binding) => {
  const { ConstructorExceptionTest } = binding.objectwrapConstructorException;
  assert.throws(() => (new ConstructorExceptionTest()), /an exception/);
  global.gc();
}

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));
