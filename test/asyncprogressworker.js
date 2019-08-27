'use strict';
const buildType = process.config.target_defaults.default_configuration;
const common = require('./common')
const assert = require('assert');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test({ asyncprogressworker }) {
  success(asyncprogressworker);
  fail(asyncprogressworker);
  return;
}

function success(binding) {
  const expected = [0, 1, 2, 3];
  const actual = [];
  binding.doWork(expected.length,
    common.mustCall((err) => {
      if (err) {
        assert.fail(err);
      }
    }),
    common.mustCall((_progress) => {
      actual.push(_progress);
      if (actual.length === expected.length) {
        assert.deepEqual(actual, expected);
      }
    }, expected.length)
  );
}

function fail(binding) {
  binding.doWork(-1,
    common.mustCall((err) => {
      assert.throws(() => { throw err }, /test error/)
    }),
    () => {
      assert.fail('unexpected progress report');
    }
  );
}
