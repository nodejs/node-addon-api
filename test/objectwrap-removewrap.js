'use strict';

if (process.argv[2] === 'child') {
  // Create a single wrapped instance then exit.
  return new (require(process.argv[3]).objectwrap.Test)();
}

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const { spawnSync } = require('child_process');

const test = (bindingName) => {
  const binding = require(bindingName);
  const Test = binding.objectwrap_removewrap.Test;
  const getDtorCalled = binding.objectwrap_removewrap.getDtorCalled;

  assert.strictEqual(getDtorCalled(), 0);
  assert.throws(() => {
    new Test();
  });
  assert.strictEqual(getDtorCalled(), 1);
  global.gc();  // Does not crash.

  // Start a child process that creates a single wrapped instance to ensure that
  // it is properly freed at its exit. It must not segfault.
  // Re: https://github.com/nodejs/node-addon-api/issues/660
  const child = spawnSync(process.execPath, [
    __filename, 'child', bindingName
  ]);
  assert.strictEqual(child.signal, null);
  assert.strictEqual(child.status, 0);
}

test(`./build/${buildType}/binding.node`);
test(`./build/${buildType}/binding_noexcept.node`);
