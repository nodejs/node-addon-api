'use strict';

const assert = require('assert');
const testUtil = require('./testUtil');
const safeBuffer = require('safe-buffer');

module.exports = require('./common').runTest(test);

function test(binding) {
  return testUtil.runGCTests([
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
    },
    () => {
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
    },
    () => {
        assert.strictEqual(1, binding.buffer.getFinalizeCount());
    },
  ]);
}
