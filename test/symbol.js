'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  const {
    InitializeSymbol,
    IsSymbol
  } = binding.symbol;

  // test for InitializeSymbol
  assert.strictEqual(typeof InitializeSymbol(), 'symbol');
  assert.strictEqual(typeof InitializeSymbol('sym'), 'symbol');
  
  // test for IsSymbol
  assert.ok(IsSymbol(InitializeSymbol()));
  assert.ok(IsSymbol(InitializeSymbol('sym')));
}
