'use strict';

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));

function createWeakRef(binding, bindingToTest) {
  return binding.object[bindingToTest]({});
}

function test(binding) {
  const obj1 = createWeakRef(binding, 'addFinalizer');
  global.gc();
  assert.deepStrictEqual(obj1, { finalizerCalled: true });

  const obj2 = createWeakRef(binding, 'addFinalizerWithHint');
  global.gc();
  assert.deepStrictEqual(obj2, { finalizerCalledWithCorrectHint: true });
}
