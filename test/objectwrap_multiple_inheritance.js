'use strict';

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

const test = bindingName => {
  const binding = require(bindingName);
  const TestMI = binding.objectwrap_multiple_inheritance.TestMI;
  const testmi = new TestMI();

  assert.strictEqual(testmi.test, 0);
}

test(`./build/${buildType}/binding.node`);
test(`./build/${buildType}/binding_noexcept.node`);
