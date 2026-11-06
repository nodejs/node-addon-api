'use strict';

const assert = require('assert');
const { whichBuildType } = require('./common');

// These tests ensure that AsyncProgressWorker can be used in a terminating
// environment without triggering any crashes, use-after-free, or fatal errors.

if (process.argv[2] === 'runInChildProcess') {
  const bindingPath = process.argv[3];
  const testCase = Number(process.argv[4]);

  const binding = require(bindingPath);

  binding.asyncprogressworker.resetPromises();

  const { Worker } = require('worker_threads');

  const worker = new Worker(
    __filename,
    {
      argv: [
        'runInWorkerThread',
        bindingPath,
        testCase
      ]
    }
  );

  if (testCase === 0) {
    // Test case 0: Environment teardown occurs while worker is actively running
    binding.asyncprogressworker.waitForWorkerThread();
    worker.terminate();
    binding.asyncprogressworker.releaseWorkerThread();
  } else if (testCase === 1) {
    // Test case 1: Worker finishes normally, then environment is torn down
    // (Verifies cleanup hook was safely removed and does not trigger use-after-free on teardown)
    worker.on('exit', () => {
      process.exit(0);
    });
  }
} else if (process.argv[2] === 'runInWorkerThread') {
  const bindingPath = process.argv[3];
  const testCase = Number(process.argv[4]);

  const binding = require(bindingPath);

  if (testCase === 0) {
    binding.asyncprogressworker.doTerminatingWork(
      () => {},
      () => {}
    );
  } else if (testCase === 1) {
    binding.asyncprogressworker.doWork(
      1,
      () => {},
      () => {}
    );
  }
} else {
  module.exports = wrapTest();

  async function wrapTest () {
    const path = require('path');
    const buildType = await whichBuildType();
    test(path.join(__dirname, `build/${buildType}/binding.node`));
    test(path.join(__dirname, `build/${buildType}/binding_noexcept.node`));
  }

  function test (bindingPath) {
    const numberOfTestCases = 2;

    for (let i = 0; i < numberOfTestCases; ++i) {
      const childProcess = require('./napi_child').spawnSync(
        process.execPath,
        [
          __filename,
          'runInChildProcess',
          bindingPath,
          i
        ]
      );

      assert.strictEqual(
        childProcess.status,
        0,
        `Test case ${bindingPath} ${i} failed with status ${childProcess.status}: ${childProcess.stderr}`
      );
    }
  }
}
