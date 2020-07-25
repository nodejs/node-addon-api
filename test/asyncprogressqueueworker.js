'use strict';
const buildType = process.config.target_defaults.default_configuration;
const common = require('./common')
const assert = require('assert');
const os = require('os');

module.exports = test(require(`./build/${buildType}/binding.node`))
  .then(() => test(require(`./build/${buildType}/binding_noexcept.node`)));

async function test({ asyncprogressqueueworker }) {
  await success(asyncprogressqueueworker);
  await fail(asyncprogressqueueworker);
  await cancel(asyncprogressqueueworker);
}

function success(binding) {
  return new Promise((resolve, reject) => {
    const expected = [0, 1, 2, 3];
    const actual = [];
    const worker = binding.createWork(expected.length,
      common.mustCall((err) => {
        if (err) {
          reject(err);
        } else {
          // All queued items shall be invoked before complete callback.
          assert.deepEqual(actual, expected);
          resolve();
        }
      }),
      common.mustCall((_progress) => {
        actual.push(_progress);
      }, expected.length)
    );
    binding.queueWork(worker);
  });
}

function fail(binding) {
  return new Promise((resolve, reject) => {
    const worker = binding.createWork(-1,
      common.mustCall((err) => {
        assert.throws(() => { throw err }, /test error/);
        resolve();
      }),
      common.mustNotCall()
    );
    binding.queueWork(worker);
  });
}

function cancel(binding) {
  return new Promise((resolve, reject) => {
    // make sure the work we are going to cancel will not be
    // able to start by using all the threads in the pool.
    for (let i = 0; i < os.cpus().length; ++i) {
      const worker = binding.createWork(-1, () => {}, () => {});
      binding.queueWork(worker);
    }
    const worker = binding.createWork(-1,
      () => {
        assert.fail('unexpected callback');
      },
      () => {
        assert.fail('unexpected progress report');
      }
    );
    binding.cancelWork(worker);
    resolve();
  });
}
