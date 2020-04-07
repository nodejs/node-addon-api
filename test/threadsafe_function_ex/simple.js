'use strict';

const buildType = process.config.target_defaults.default_configuration;

module.exports = Promise.all([
  test(require(`../build/${buildType}/binding.node`)),
  test(require(`../build/${buildType}/binding_noexcept.node`))
]);

async function test(binding) {
  const ctx = {};
  const tsfn = new binding.threadsafe_function_ex_simple.TSFNWrap(ctx);
  tsfn.call();
  await tsfn.release();
}
