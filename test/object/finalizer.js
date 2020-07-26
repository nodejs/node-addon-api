'use strict';

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const testUtil = require('../testUtil');

module.exports = test(require(`../build/${buildType}/binding.node`))
  .then(() => test(require(`../build/${buildType}/binding_noexcept.node`)));

function createWeakRef(binding, bindingToTest) {
  return binding.object[bindingToTest]({});
}

function test(binding) {
  let obj1;
  let obj2;
  return testUtil.runGCTests([
    'addFinalizer',
    () => {
      obj1 = createWeakRef(binding, 'addFinalizer');
    },
    () => assert.deepStrictEqual(obj1, { finalizerCalled: true }),

    'addFinalizerWithHint',
    () => {
      obj2 = createWeakRef(binding, 'addFinalizerWithHint');
    },
    () => assert.deepStrictEqual(obj2, { finalizerCalledWithCorrectHint: true })
  ]);
}
