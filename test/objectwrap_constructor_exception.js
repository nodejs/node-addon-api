'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

const test = (binding) => {
  const { ConstructorExceptionTest } = binding.objectwrapConstructorException;
  let gotException = false;
  try {
    new ConstructorExceptionTest();
  } catch (anException) {
    gotException = true;
  }
  global.gc();

  assert.strictEqual(gotException, true);
}

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));
