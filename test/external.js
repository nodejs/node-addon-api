'use strict';
const assert = require('assert');
const testUtil = require('./testUtil');

test(require('./load-bindings')('binding'));
test(require('./load-bindings')('binding_noexcept'));

function test(binding) {
  testUtil.runGCTests([
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
