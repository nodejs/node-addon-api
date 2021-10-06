'use strict';

const common = require('./common');
const assert = require('assert');

module.exports = common.runTest(test);

async function test ({ asyncprogressworker }) {
  await success(asyncprogressworker);
  await fail(asyncprogressworker);
  await malignTest(asyncprogressworker);
  await signalTest(asyncprogressworker);
}

function success (binding) {
  return new Promise((resolve, reject) => {
    const expected = [0, 1, 2, 3];
    const actual = [];
    binding.doWork(expected.length,
      common.mustCall((err) => {
        if (err) {
          reject(err);
        }
      }),
      common.mustCall((_progress) => {
        actual.push(_progress);
        if (actual.length === expected.length) {
          assert.deepEqual(actual, expected);
          resolve();
        }
      }, expected.length)
    );
  });
}

function fail (binding) {
  return new Promise((resolve) => {
    binding.doWork(-1,
      common.mustCall((err) => {
        assert.throws(() => { throw err; }, /test error/);
        resolve();
      }),
      common.mustNotCall()
    );
  });
}

function malignTest (binding) {
  return new Promise((resolve, reject) => {
    binding.doMalignTest(
      common.mustCall((err) => {
        if (err) {
          return reject(err);
        }
        resolve();
      }),
      common.mustCallAtLeast((error, reason) => {
        assert(!error, reason);
      }, 1)
    );
  });
}

function signalTest (binding) {
  return new Promise((resolve, reject) => {
    const expectedCalls = 3;
    let actualCalls = 0;
    binding.doWork(expectedCalls,
      common.mustCall((err) => {
        if (err) {
          reject(err);
        }
      }),
      common.mustCall((_progress) => {
        actualCalls++;
        if (expectedCalls === actualCalls) {
          resolve();
        }
      }, expectedCalls)
    );
  });
}
