'use strict';

const assert = require('assert');
const fs = require('fs');
const path = require('path');
const common = require('./common');
const testUtil = require('./testUtil');

module.exports = common.runTestWithBuildType(async (buildType) => {
  await common.runTest(test, buildType);

  const cpp20BindingPath = [
    path.resolve(__dirname, `build/${buildType}/binding_cpp20.node`),
    path.resolve(__dirname, `../build/${buildType}/binding_cpp20.node`)
  ].find(p => fs.existsSync(p));

  if (cpp20BindingPath) {
    await Promise.resolve(
      test(require(cpp20BindingPath), { bindingPath: cpp20BindingPath })
    ).finally(common.mustCall());
  }
});

function test (binding, context) {
  return testUtil.runGCTests([
    'Internal Buffer',
    () => {
      const test = binding.buffer.createBuffer();
      binding.buffer.checkBuffer(test);
      assert.ok(test instanceof Buffer);

      const test2 = Buffer.alloc(test.length);
      test.copy(test2);
      binding.buffer.checkBuffer(test2);
    },

    'Buffer copy',
    () => {
      const test = binding.buffer.createBufferCopy();
      binding.buffer.checkBuffer(test);
      assert.ok(test instanceof Buffer);
    },

    'std::span Buffer tests',
    () => {
      if (context && context.bindingPath && context.bindingPath.includes('binding_cpp20')) {
        assert.strictEqual(binding.buffer.hasSpanSupport, true);
      }
      if (!binding.buffer.hasSpanSupport) return;

      const fromSpan = binding.buffer.createBufferFromSpan();
      binding.buffer.checkBuffer(fromSpan);
      assert.ok(fromSpan instanceof Buffer);
      assert.ok(binding.buffer.checkBufferSpan(fromSpan));

      const copyFromSpan = binding.buffer.createBufferCopyFromSpan();
      binding.buffer.checkBuffer(copyFromSpan);
      assert.ok(copyFromSpan instanceof Buffer);
      assert.ok(binding.buffer.checkBufferSpan(copyFromSpan));

      const newOrCopySpan = binding.buffer.createBufferNewOrCopyFromSpan();
      binding.buffer.checkBuffer(newOrCopySpan);
      assert.ok(newOrCopySpan instanceof Buffer);
      assert.ok(binding.buffer.checkBufferSpan(newOrCopySpan));
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

    'Create or Copy External Buffer',
    () => {
      const test = binding.buffer.createOrCopyExternalBuffer();
      binding.buffer.checkBuffer(test);
      assert.ok(test instanceof Buffer);
      assert.strictEqual(0, binding.buffer.getFinalizeCount());
    },
    () => {
      global.gc();
      assert.strictEqual(0, binding.buffer.getFinalizeCount());
    },

    'Create or Copy External Buffer with finalizer',
    () => {
      const test = binding.buffer.createOrCopyExternalBufferWithFinalize();
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

    'Create or Copy External Buffer with finalizer hint',
    () => {
      const test = binding.buffer.createOrCopyExternalBufferWithFinalizeHint();
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

    'Create or Copy External Buffer when NODE_API_NO_EXTERNAL_BUFFERS_ALLOWED defined',
    () => {
      const test = binding.bufferNoExternal.createOrCopyExternalBuffer();
      binding.buffer.checkBuffer(test);
      assert.ok(test instanceof Buffer);
      assert.strictEqual(0, binding.buffer.getFinalizeCount());
    },
    () => {
      global.gc();
      assert.strictEqual(0, binding.buffer.getFinalizeCount());
    },

    'Create or Copy External Buffer with finalizer when NODE_API_NO_EXTERNAL_BUFFERS_ALLOWED defined',
    () => {
      const test = binding.bufferNoExternal.createOrCopyExternalBufferWithFinalize();
      binding.buffer.checkBuffer(test);
      assert.ok(test instanceof Buffer);
      // finalizer should have been called when the buffer was created.
      assert.strictEqual(1, binding.buffer.getFinalizeCount());
    },
    () => {
      global.gc();
    },
    () => {
      assert.strictEqual(1, binding.buffer.getFinalizeCount());
    },

    'Create or Copy External Buffer with finalizer hint when NODE_API_NO_EXTERNAL_BUFFERS_ALLOWED defined',
    () => {
      const test = binding.bufferNoExternal.createOrCopyExternalBufferWithFinalizeHint();
      binding.buffer.checkBuffer(test);
      assert.ok(test instanceof Buffer);
      // finalizer should have been called when the buffer was created.
      assert.strictEqual(1, binding.buffer.getFinalizeCount());
    },
    () => {
      global.gc();
    },
    () => {
      assert.strictEqual(1, binding.buffer.getFinalizeCount());
    }
  ]);
}
