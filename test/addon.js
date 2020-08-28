'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  assert.strictEqual(binding.addon.increment(), 43);
  assert.strictEqual(binding.addon.increment(), 44);
  assert.strictEqual(binding.addon.subObject.decrement(), 43);
}
