'use strict';

const assert = require('assert');
const testUtil = require('./testUtil');

function test (binding) {
  return testUtil.runGCTests([
    'objectwrap function',
    () => {
      const { FunctionTest } = binding.objectwrap_function;
      const newConstructed = new FunctionTest();
      const functionConstructed = FunctionTest();
      assert(newConstructed instanceof FunctionTest);
      assert(functionConstructed instanceof FunctionTest);
      assert.throws(() => (FunctionTest(true)), /an exception/);
    },
    // Do on gc before returning.
    () => {}
  ]);
}

module.exports = require('./common').runTest(test);
