'use strict';

const assert = require('assert');

module.exports = require('./common').runTest(binding => {
  test(binding.functionreference);
});

function test(binding) {
  const e = new Error('foobar');
  const functionMayThrow = () => { throw e; };
  const classMayThrow = class { constructor() { throw e; } };

  assert.throws(() => {
    binding.call(functionMayThrow);
  }, /foobar/);
  assert.throws(() => {
    binding.construct(classMayThrow);
  }, /foobar/);
}
