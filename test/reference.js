'use strict';


const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const testUtil = require('./testUtil');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  binding.reference.createWeakArray();
  global.gc();
  assert.strictEqual(true, binding.reference.accessWeakArrayEmpty());
};
