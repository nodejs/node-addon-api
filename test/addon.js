'use strict';

const assert = require('assert');

module.exports = require('./common').runTest(test);

function test(binding) {
  assert.strictEqual(binding.addon.increment(), 43);
  assert.strictEqual(binding.addon.increment(), 44);
  assert.strictEqual(binding.addon.subObject.decrement(), 43);
}
