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
        assert.strictEqual(binding.finalizer_order.Test.isBasicFinalizerCalled, true, 'Expected basic finalizer to be called [before ticking]');
        assert.strictEqual(binding.finalizer_order.Test.isFinalizerCalled, false, 'Expected (extended) finalizer to not be called [before ticking]');
        assert.strictEqual(isCallbackCalled, false, 'Expected callback to not be called [before ticking]');
      } else {
        assert.strictEqual(binding.finalizer_order.Test.isBasicFinalizerCalled, false, 'Expected basic finalizer to not be called [before ticking]');
        assert.strictEqual(binding.finalizer_order.Test.isFinalizerCalled, false, 'Expected (extended) finalizer to not be called [before ticking]');
        assert.strictEqual(isCallbackCalled, false, 'Expected callback to not be called [before ticking]');
      }
    },
    () => {
      assert.strictEqual(binding.finalizer_order.Test.isBasicFinalizerCalled, true, 'Expected basic finalizer to be called [after ticking]');
      assert.strictEqual(binding.finalizer_order.Test.isFinalizerCalled, true, 'Expected (extended) finalizer to be called [after ticking]');
      assert.strictEqual(isCallbackCalled, true, 'Expected callback to be called [after ticking]');
    },

    'Finalizer Order - External with Basic Finalizer',
    () => {
      let ext = binding.finalizer_order.createExternalBasicFinalizer();
      ext = null;
      global.gc();

      if (isExperimental) {
        assert.strictEqual(binding.finalizer_order.isExternalBasicFinalizerCalled(), true, 'Expected External basic finalizer to be called [before ticking]');
      } else {
        assert.strictEqual(binding.finalizer_order.isExternalBasicFinalizerCalled(), false, 'Expected External basic finalizer to not be called [before ticking]');
      }
    },
    () => {
      assert.strictEqual(binding.finalizer_order.isExternalBasicFinalizerCalled(), true, 'Expected External basic finalizer to be called [after ticking]');
    },

    'Finalizer Order - External with Finalizer',
    () => {
      let ext = binding.finalizer_order.createExternalFinalizer();
      ext = null;
      global.gc();
      assert.strictEqual(binding.finalizer_order.isExternalFinalizerCalled(), false, 'Expected External extended finalizer to not be called [before ticking]');
    },
    () => {
      assert.strictEqual(binding.finalizer_order.isExternalFinalizerCalled(), true, 'Expected External extended finalizer to be called [after ticking]');
    }
  ];

  if (binding.isExperimental) {
    tests.push(...[
      'PostFinalizer',
      () => {
        binding.finalizer_order.PostFinalizer(common.mustCall());
      },

      'PostFinalizerWithData',
      () => {
        const data = {};
        const callback = (callbackData) => {
          assert.strictEqual(callbackData, data);
        };
        binding.finalizer_order.PostFinalizerWithData(common.mustCall(callback), data);
      },

      'PostFinalizerWithDataAndHint',
      () => {
        const data = {};
        const hint = {};
        const callback = (callbackData, callbackHint) => {
          assert.strictEqual(callbackData, data);
          assert.strictEqual(callbackHint, hint);
        };
        binding.finalizer_order.PostFinalizerWithDataAndHint(common.mustCall(callback), data, hint);
      }
    ]);
  }

  return testUtil.runGCTests(tests);
}
