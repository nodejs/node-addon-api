'use strict';
const assert = require('assert');

test(require('./load-bindings')('binding'));
test(require('./load-bindings')('binding_noexcept'));

function test(binding) {
  binding.asyncworker.doWork(true, function (e) {
    assert.strictEqual(typeof e, 'undefined');
    assert.strictEqual(typeof this, 'object');
    assert.strictEqual(this.data, 'test data');
  }, 'test data');

  binding.asyncworker.doWork(false, function (e) {
    assert.ok(e instanceof Error);
    assert.strictEqual(e.message, 'test error');
    assert.strictEqual(typeof this, 'object');
    assert.strictEqual(this.data, 'test data');
  }, 'test data');
}
