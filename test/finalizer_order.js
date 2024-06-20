'use strict';

/* eslint-disable no-unused-vars */

const assert = require('assert');
const common = require('./common');
const testUtil = require('./testUtil');

module.exports = require('./common').runTest(test);

function test (binding) {
  const { isExperimental } = binding;

  let isCallbackCalled = false;

  const tests = [
    'Finalizer Order - ObjectWrap',
    () => {
      let test = new binding.finalizer_order.Test(() => { isCallbackCalled = true; });
      test = null;

      global.gc();

      if (isExperimental) {
        assert.strictEqual(binding.finalizer_order.Test.isNogcFinalizerCalled, true, 'Expected nogc finalizer to be called [before ticking]');
        assert.strictEqual(binding.finalizer_order.Test.isGcFinalizerCalled, false, 'Expected gc finalizer to not be called [before ticking]');
        assert.strictEqual(isCallbackCalled, false, 'Expected callback not be called [before ticking]');
      } else {
        assert.strictEqual(binding.finalizer_order.Test.isNogcFinalizerCalled, false, 'Expected nogc finalizer to not be called [before ticking]');
        assert.strictEqual(binding.finalizer_order.Test.isGcFinalizerCalled, false, 'Expected gc finalizer to not be called [before ticking]');
        assert.strictEqual(isCallbackCalled, false, 'Expected callback not be called [before ticking]');
      }
    },
    () => {
      assert.strictEqual(binding.finalizer_order.Test.isNogcFinalizerCalled, true, 'Expected nogc finalizer to be called [after ticking]');
      assert.strictEqual(binding.finalizer_order.Test.isGcFinalizerCalled, true, 'Expected gc finalizer to be called [after ticking]');
      assert.strictEqual(isCallbackCalled, true, 'Expected callback to be called [after ticking]');
    },

    'Finalizer Order - External with Nogc Finalizer',
    () => {
      let ext = new binding.finalizer_order.CreateExternalNogc();
      ext = null;
      global.gc();

      if (isExperimental) {
        assert.strictEqual(binding.finalizer_order.isExternalNogcFinalizerCalled(), true, 'Expected External nogc finalizer to be called [before ticking]');
      } else {
        assert.strictEqual(binding.finalizer_order.isExternalNogcFinalizerCalled(), false, 'Expected External nogc finalizer to not be called [before ticking]');
      }
    },
    () => {
      assert.strictEqual(binding.finalizer_order.isExternalNogcFinalizerCalled(), true, 'Expected External nogc finalizer to be called [after ticking]');
    },

    'Finalizer Order - External with Gc Finalizer',
    () => {
      let ext = new binding.finalizer_order.CreateExternalGc();
      ext = null;
      global.gc();
      assert.strictEqual(binding.finalizer_order.isExternalGcFinalizerCalled(), false, 'Expected External gc finalizer to not be called [before ticking]');
    },
    () => {
      assert.strictEqual(binding.finalizer_order.isExternalGcFinalizerCalled(), true, 'Expected External gc finalizer to be called [after ticking]');
    }
  ];

  if (binding.isExperimental) {
    tests.push(...[
      'AddPostFinalizer',
      () => {
        binding.finalizer_order.AddPostFinalizer(common.mustCall());
      },

      'AddPostFinalizerWithData',
      () => {
        const data = {};
        const callback = (callbackData) => {
          assert.strictEqual(callbackData, data);
        };
        binding.finalizer_order.AddPostFinalizerWithData(common.mustCall(callback), data);
      },

      'AddPostFinalizerWithDataAndHint',
      () => {
        const data = {};
        const hint = {};
        const callback = (callbackData, callbackHint) => {
          assert.strictEqual(callbackData, data);
          assert.strictEqual(callbackHint, hint);
        };
        binding.finalizer_order.AddPostFinalizerWithDataAndHint(common.mustCall(callback), data, hint);
      }
    ]);
  }

  return testUtil.runGCTests(tests);
}
