'use strict';

const assert = require('assert');
const buildType = process.config.target_defaults.default_configuration;

module.exports = Promise.all([
  test(require(`../build/${buildType}/binding.node`)),
  // test(require(`../build/${buildType}/binding_noexcept.node`))
]);

async function test(binding) {
  const ctx = { };
  const tsfn = new binding.threadsafe_function_ex.TSFNWrap(ctx);
  console.log("1");
  assert(ctx === await tsfn.getContextByCall(),"getContextByCall context not equal");
  console.log("2");
  assert(ctx === tsfn.getContextFromTsfn(),"getContextFromTsfn context not equal");
  console.log("3");
  console.log("releasing");
  await tsfn.release();
  console.log("released!");
}
