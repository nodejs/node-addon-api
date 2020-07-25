'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const testUtil = require('./testUtil');

module.exports = test(require(`./build/${buildType}/binding.node`))
  .then(() => test(require(`./build/${buildType}/binding_noexcept.node`)));

function test(binding) {
  return testUtil.runGCTests([
    'External without finalizer',
    () => {
      const test = binding.external.createExternal();
      assert.strictEqual(typeof test, 'object');
      binding.external.checkExternal(test);
      assert.strictEqual(0, binding.external.getFinalizeCount());
    },
    () => {
      assert.strictEqual(0, binding.external.getFinalizeCount());
    },

    'External with finalizer',
    () => {
      const test = binding.external.createExternalWithFinalize();
      assert.strictEqual(typeof test, 'object');
      binding.external.checkExternal(test);
      assert.strictEqual(0, binding.external.getFinalizeCount());
    },
    () => {
      assert.strictEqual(1, binding.external.getFinalizeCount());
    },

    'External with finalizer hint',
    () => {
      const test = binding.external.createExternalWithFinalizeHint();
      assert.strictEqual(typeof test, 'object');
      binding.external.checkExternal(test);
      assert.strictEqual(0, binding.external.getFinalizeCount());
    },
    () => {
      assert.strictEqual(1, binding.external.getFinalizeCount());
    },
  ]);
}
