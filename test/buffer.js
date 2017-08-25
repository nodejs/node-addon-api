'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const testUtil = require('./testUtil');
const safeBuffer = require('safe-buffer');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  testUtil.runGCTests([
    'Internal Buffer',
    () => {
      const test = binding.buffer.createBuffer();
      binding.buffer.checkBuffer(test);
      assert.ok(test instanceof Buffer);

      const test2 = safeBuffer.Buffer.alloc(test.length);
      test.copy(test2);
      binding.buffer.checkBuffer(test2);
    },

    'Buffer copy',
    () => {
      const test = binding.buffer.createBufferCopy();
      binding.buffer.checkBuffer(test);
      assert.ok(test instanceof Buffer);
    },

    'External Buffer',
    () => {
      const test = binding.buffer.createExternalBuffer();
      binding.buffer.checkBuffer(test);
      assert.ok(test instanceof Buffer);
      assert.strictEqual(0, binding.buffer.getFinalizeCount());
    },
    () => {
        global.gc();
        assert.strictEqual(0, binding.buffer.getFinalizeCount());
    },

    'External Buffer with finalizer',
    () => {
      const test = binding.buffer.createExternalBufferWithFinalize();
      binding.buffer.checkBuffer(test);
      assert.ok(test instanceof Buffer);
      assert.strictEqual(0, binding.buffer.getFinalizeCount());
    },
    () => {
        global.gc();
        assert.strictEqual(1, binding.buffer.getFinalizeCount());
    },

    'External Buffer with finalizer hint',
    () => {
        const test = binding.buffer.createExternalBufferWithFinalizeHint();
        binding.buffer.checkBuffer(test);
        assert.ok(test instanceof Buffer);
        assert.strictEqual(0, binding.buffer.getFinalizeCount());
    },
    () => {
        global.gc();
        assert.strictEqual(1, binding.buffer.getFinalizeCount());
    },
  ]);
}
