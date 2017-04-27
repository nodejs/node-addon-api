'use strict';
const buildType = process.config.target_defaults.default_configuration;
const binding = require(`./build/${buildType}/binding.node`);
const assert = require('assert');

// Use setTimeout() when asserting after async callbacks because
// unhandled JS exceptions in async callbacks are currently ignored.
// See the TODO comment in AsyncWorker::OnWorkComplete().

binding.asyncworker.doWork(true, function (e) {
  setTimeout(() => {
    assert.strictEqual(typeof e, 'undefined');
    assert.strictEqual(typeof this, 'object');
    assert.strictEqual(this.data, 'test data');
  });
}, 'test data');

binding.asyncworker.doWork(false, function (e) {
  setTimeout(() => {
    assert.ok(e instanceof Error);
    assert.strictEqual(e.message, 'test error');
    assert.strictEqual(typeof this, 'object');
    assert.strictEqual(this.data, 'test data');
  });
}, 'test data');
