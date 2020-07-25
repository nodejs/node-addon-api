'use strict';


const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const testUtil = require('./testUtil');

module.exports = test(require(`./build/${buildType}/binding.node`))
  .then(() => test(require(`./build/${buildType}/binding_noexcept.node`)));

async function test(binding) {
  await testUtil.runGCTests([
    'test reference',
    () => binding.reference.createWeakArray(),
    () => assert.strictEqual(true, binding.reference.accessWeakArrayEmpty())
  ]);
};
