'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const testUtil = require('./testUtil');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  testUtil.runGCTests([
    'Internal ArrayBuffer',
    () => {
      const test = binding.arraybuffer.createBuffer();
      binding.arraybuffer.checkBuffer(test);
      assert.ok(test instanceof ArrayBuffer);

      const test2 = test.slice(0);
      binding.arraybuffer.checkBuffer(test2);
    },

    'External ArrayBuffer',
    () => {
      const test = binding.arraybuffer.createExternalBuffer();
      binding.arraybuffer.checkBuffer(test);
      assert.ok(test instanceof ArrayBuffer);
      assert.strictEqual(0, binding.arraybuffer.getFinalizeCount());
    },
    () => {
      global.gc();
      assert.strictEqual(0, binding.arraybuffer.getFinalizeCount());
    },

    'External ArrayBuffer with finalizer',
    () => {
      const test = binding.arraybuffer.createExternalBufferWithFinalize();
      binding.arraybuffer.checkBuffer(test);
      assert.ok(test instanceof ArrayBuffer);
      assert.strictEqual(0, binding.arraybuffer.getFinalizeCount());
    },
    () => {
      global.gc();
      assert.strictEqual(1, binding.arraybuffer.getFinalizeCount());
    },

    'External ArrayBuffer with finalizer hint',
    () => {
      const test = binding.arraybuffer.createExternalBufferWithFinalizeHint();
      binding.arraybuffer.checkBuffer(test);
      assert.ok(test instanceof ArrayBuffer);
      assert.strictEqual(0, binding.arraybuffer.getFinalizeCount());
    },
    () => {
      global.gc();
      assert.strictEqual(1, binding.arraybuffer.getFinalizeCount());
    },
  ]);
}
