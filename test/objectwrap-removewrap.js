'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

const test = (binding) => {
  const Test = binding.objectwrap_removewrap.Test;
  const getDtorCalled = binding.objectwrap_removewrap.getDtorCalled;

  assert.strictEqual(getDtorCalled(), 0);
  assert.throws(() => {
    new Test();
  });
  assert.strictEqual(getDtorCalled(), 1);
  global.gc();  // Does not crash.
}

test(require(`./build/${buildType}/binding.node`));
