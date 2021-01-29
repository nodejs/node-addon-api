'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  const {
    IsWholeNumber,
    EvenOrOdd
  } = binding.even_odd;

  // test for IsWholeNumber
  assert.strictEqual(IsWholeNumber(-21), false);
  assert.strictEqual(IsWholeNumber(1), true);
  
  // test for EvenOrOdd
  assert.strictEqual(EvenOrOdd(3),"Odd");
  assert.strictEqual(EvenOrOdd(64),"Even");
}
