'use strict';

const assert = require('assert');
const buildType = process.config.target_defaults.default_configuration;

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
  test(`../build/${buildType}/binding.node`);
  test(`../build/${buildType}/binding_noexcept.node`);
} else {
  test(process.argv[2]);
}

function test(bindingFile) {
  if (isMainProcess) {
    // Main process
    const child = require('../napi_child').spawn(process.argv[0], [ '--expose-gc', __filename, bindingFile ], {
      stdio: 'inherit',
    });

    let timeout = setTimeout( function() {
      child.kill();
      timeout = 0;
      throw new Error("Expected child to die");
    }, 5000);

    child.on("error", (err) => {
      clearTimeout(timeout);
      timeout = 0;
      throw new Error(err);
    })

    child.on("close", (code) => {
      if (timeout) clearTimeout(timeout);
      assert(!code, "Expected return value 0");
    });

  } else {
    // Child process
    const binding = require(bindingFile);
    binding.threadsafe_function_unref.testUnref({}, () => { });
  }
}
