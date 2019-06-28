'use strict';

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  const {
    CreateDate,
    IsDate,
    ValueOf,
    OperatorValue,
  } = binding.date;
  assert.deepStrictEqual(CreateDate(0), new Date(0));
  assert.strictEqual(IsDate(new Date(0)), true);
  assert.strictEqual(ValueOf(new Date(42)), 42);
  assert.strictEqual(OperatorValue(new Date(42)), true);
}
