'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const common = require('./common');
const binding = require(`./build/${buildType}/binding.node`);
const noexceptBinding = require(`./build/${buildType}/binding_noexcept.node`);

function test(binding, succeed) {
  return new Promise((resolve) =>
    // Can't pass an arrow function to doWork because that results in an
    // undefined context inside its body when the function gets called.
    binding.doWork(succeed, function(e) {
      setImmediate(() => {
        // If the work is supposed to fail, make sure there's an error.
        assert.strictEqual(succeed || e.message === 'test error', true);
        assert.strictEqual(binding.workerGone, false);
        binding.deleteWorker();
        assert.strictEqual(binding.workerGone, true);
        resolve();
      });
    }));
}

test(binding.persistentasyncworker, false)
  .then(() => test(binding.persistentasyncworker, true))
  .then(() => test(noexceptBinding.persistentasyncworker, false))
  .then(() => test(noexceptBinding.persistentasyncworker, true));
