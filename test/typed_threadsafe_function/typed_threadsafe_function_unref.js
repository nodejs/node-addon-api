'use strict';

const assert = require('assert');

const isMainProcess = process.argv[1] != __filename;

/**
 * In order to test that the event loop exits even with an active TSFN, we need
 * to spawn a new process for the test.
 *   - Main process: spawns new node instance, executing this script
 *   - Child process: creates TSFN. Native module Unref's via setTimeout after some time but does NOT call Release.
 *
 * Main process should expect child process to exit.
 */

if (isMainProcess) {
  module.exports = require('../common').runTestWithBindingPath(test);
} else {
  test(process.argv[2]);
}

function test(bindingFile) {
  if (isMainProcess) {
    // Main process
    return new Promise((resolve, reject) => {
      const child = require('../napi_child').spawn(process.argv[0], [
        '--expose-gc', __filename, bindingFile
      ], { stdio: 'inherit' });

      let timeout = setTimeout( function() {
        child.kill();
        timeout = 0;
        reject(new Error("Expected child to die"));
      }, 5000);

      child.on("error", (err) => {
        clearTimeout(timeout);
        timeout = 0;
        reject(new Error(err));
      })

      child.on("close", (code) => {
        if (timeout) clearTimeout(timeout);
        assert.strictEqual(code, 0, "Expected return value 0");
        resolve();
      });
    });
  } else {
    // Child process
    const binding = require(bindingFile);
    binding.typed_threadsafe_function_unref.testUnref({}, () => { });
  }
}
