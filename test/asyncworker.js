'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

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
