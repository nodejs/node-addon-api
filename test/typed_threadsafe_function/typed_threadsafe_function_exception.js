'use strict';

const assert = require('assert');
const common = require('../common');

const { spawnSync } = require('../napi_child');

function test (bindingPath) {
  const { status } = spawnSync(
    process.execPath,
    [
      '--force-node-api-uncaught-exceptions-policy=true',
      __filename,
      'child',
      bindingPath
    ],
    { stdio: 'inherit' }
  );

  assert.strictEqual(status, 0);
}

if (process.argv[2] === 'child') {
  child(process.argv[3])
    .catch(err => {
      process.exitCode = 1;
      console.error(err);
    });
} else {
  module.exports = common.runTestWithBindingPath(test);
}

async function child (bindingPath) {
  const binding = require(bindingPath);
  const { testCall } = binding.typed_threadsafe_function_exception;

  await new Promise(resolve => {
    process.once('uncaughtException', common.mustCall(err => {
      assert.strictEqual(err.message, 'test-from-native');
      resolve();
    }, 1));

    testCall();
  });
}
