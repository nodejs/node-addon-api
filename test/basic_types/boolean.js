'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));

function test(binding) {
  const bool1 = binding.basic_types_boolean.createBoolean(true);
  assert.strictEqual(bool1, true);

  const bool2 = binding.basic_types_boolean.createBoolean(false);
  assert.strictEqual(bool2, false);
}
