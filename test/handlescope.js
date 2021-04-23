'use strict';

const assert = require('assert');

module.exports = require('./common').runTest(test);

function test(binding) {
  assert.strictEqual(binding.handlescope.createScope(), 'scope');
  assert.strictEqual(binding.handlescope.escapeFromScope(), 'inner-scope');
  assert.strictEqual(binding.handlescope.stressEscapeFromScope(), 'inner-scope999999');
  assert.throws(() => binding.handlescope.doubleEscapeFromScope(),
                Error,
                ' napi_escape_handle already called on scope');
}
