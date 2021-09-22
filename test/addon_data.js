'use strict';

const assert = require('assert');
const { spawn } = require('child_process');
const readline = require('readline');

module.exports = require('./common').runTestWithBindingPath(test);

// Make sure the instance data finalizer is called at process exit. If the hint
// is non-zero, it will be printed out by the child process.
function testFinalizer (bindingName, hint, expected) {
  return new Promise((resolve) => {
    bindingName = bindingName.split('\\').join('\\\\');
    const child = spawn(process.execPath, [
      '-e',
      `require('${bindingName}').addon_data(${hint}).verbose = true;`
    ]);
    const actual = [];
    readline
      .createInterface({ input: child.stderr })
      .on('line', (line) => {
        if (expected.indexOf(line) >= 0) {
          actual.push(line);
        }
      })
      .on('close', () => {
        assert.deepStrictEqual(expected, actual);
        resolve();
      });
  });
}

async function test (bindingName) {
  const binding = require(bindingName).addon_data(0);

  // Make sure it is possible to get/set instance data.
  assert.strictEqual(binding.verbose.verbose, false);
  binding.verbose = true;
  assert.strictEqual(binding.verbose.verbose, true);
  binding.verbose = false;
  assert.strictEqual(binding.verbose.verbose, false);

  await testFinalizer(bindingName, 0, ['addon_data: Addon::~Addon']);
  await testFinalizer(bindingName, 42,
    ['addon_data: Addon::~Addon', 'hint: 42']);
}
