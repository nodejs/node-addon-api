'use strict';

/**
 * This test is programmatically represents the example shown in
 * `doc/threadsafe_function_ex.md`
 */

const assert = require('assert');
const buildType = process.config.target_defaults.default_configuration;

module.exports = Promise.all([
  test(require(`../build/${buildType}/binding.node`)),
  test(require(`../build/${buildType}/binding_noexcept.node`))
]);

async function test(binding) {
  const tsfn = new binding.threadsafe_function_ex_example.TSFNWrap();
  await tsfn.start();
  await tsfn.release();
}
