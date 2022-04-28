'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

// These tests ensure that Error types can be used in a terminating
// environment without triggering any fatal errors.

if (process.argv[2] === 'runInChildProcess') {
  const binding_path = process.argv[3];
  const index_for_test_case = Number(process.argv[4]);

  const binding = require(binding_path);

  // Use C++ promises to ensure the worker thread is terminated right
  // before running the testable code in the binding.

  binding.error.resetPromises();

  const { Worker } = require('worker_threads');

  const worker = new Worker(
    __filename,
    {
      argv: [
        'runInWorkerThread',
        binding_path,
        index_for_test_case
      ]
    }
  );

  binding.error.waitForWorkerThread();

  worker.terminate();

  binding.error.releaseWorkerThread();

  return;
}

if (process.argv[2] === 'runInWorkerThread') {
  const binding_path = process.argv[3];
  const index_for_test_case = Number(process.argv[4]);

  const binding = require(binding_path);

  switch (index_for_test_case) {
    case 0:
      binding.error.throwJSError('test', true);
      break;
    case 1:
      binding.error.throwTypeError('test', true);
      break;
    case 2:
      binding.error.throwRangeError('test', true);
      break;
    case 3:
      binding.error.throwDefaultError(false, true);
      break;
    case 4:
      binding.error.throwDefaultError(true, true);
      break;
    default: assert.fail('Invalid index');
  }

  assert.fail('This should not be reachable');
}

test(`./build/${buildType}/binding.node`, true);
test(`./build/${buildType}/binding_noexcept.node`, true);
test(`./build/${buildType}/binding_swallowexcept.node`, false);
test(`./build/${buildType}/binding_swallowexcept_noexcept.node`, false);
test(`./build/${buildType}/binding_custom_namespace.node`, true);

function test (bindingPath, process_should_abort) {
  const number_of_test_cases = 5;

  for (let i = 0; i < number_of_test_cases; ++i) {
    const child_process = require('./napi_child').spawnSync(
      process.execPath,
      [
        __filename,
        'runInChildProcess',
        bindingPath,
        i
      ]
    );

    if (process_should_abort) {
      assert(child_process.status !== 0, `Test case ${bindingPath} ${i} failed: Process exited with status code 0.`);
    } else {
      assert(child_process.status === 0, `Test case ${bindingPath} ${i} failed: Process status ${child_process.status} is non-zero`);
    }
  }
}
