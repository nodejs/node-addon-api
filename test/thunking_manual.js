// Flags: --expose-gc
'use strict';

const assert = require('assert');

module.exports = require('./common').runTest(test);

function test (binding) {
  console.log('Thunking: Performing initial GC');
  global.gc();
  console.log('Thunking: Creating test object');
  let object = binding.thunking_manual.createTestObject();
  object = null;
  console.log('Thunking: About to GC\n--------');
  global.gc();
  console.log('--------\nThunking: GC complete');
}
