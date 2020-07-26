'use strict';
const buildType = process.config.target_defaults.default_configuration;
const common = require('./common')
const assert = require('assert');

module.exports = test(require(`./build/${buildType}/binding.node`))
  .then(() => test(require(`./build/${buildType}/binding_noexcept.node`)));

async function test({ asyncprogressworker }) {
  await success(asyncprogressworker);
  await fail(asyncprogressworker);
}

function success(binding) {
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

function fail(binding) {
  return new Promise((resolve) => {
    binding.doWork(-1,
      common.mustCall((err) => {
        assert.throws(() => { throw err }, /test error/)
        resolve();
      }),
      common.mustNotCall()
    );
  });
}
