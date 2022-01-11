'use strict';

const assert = require('assert');
const testUtil = require('./testUtil');

function test(binding) {
  return testUtil.runGCTests([
    'objectwrap nonconstructor',
    () => {
      const { NonConstructorTest } = binding.objectwrap_nonconstructor;
      const newConstructed = new NonConstructorTest();
      const factoryConstructed = NonConstructorTest();
      assert(newConstructed instanceof NonConstructorTest);
      assert(factoryConstructed instanceof NonConstructorTest);
      assert.throws(() => (NonConstructorTest(true)), /an exception/);
    },
    // Do on gc before returning.
    () => {}
  ]);
}

module.exports = require('./common').runTest(test);
