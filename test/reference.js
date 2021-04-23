'use strict';

const assert = require('assert');
const testUtil = require('./testUtil');

module.exports = require('./common').runTest(test);

function test(binding) {
  return testUtil.runGCTests([
    'test reference',
    () => binding.reference.createWeakArray(),
    () => assert.strictEqual(true, binding.reference.accessWeakArrayEmpty())
  ]);
};
