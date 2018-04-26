'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  assert.strictEqual(binding.handlescope.createScope(), 'scope');
  assert.strictEqual(binding.handlescope.escapeFromScope(), 'inner-scope');
  assert.strictEqual(binding.handlescope.stressEscapeFromScope(), 'inner-scope999999');
  assert.throws(() => binding.handlescope.doubleEscapeFromScope(),
                Error,
                ' napi_escape_handle already called on scope');
}
