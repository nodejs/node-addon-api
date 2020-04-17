'use strict';

const assert = require('assert');
const buildType = process.config.target_defaults.default_configuration;

module.exports = Promise.all([
  test(require(`../build/${buildType}/binding.node`)),
  test(require(`../build/${buildType}/binding_noexcept.node`))
]);

async function test(binding) {
  const data = {};
  const tsfn = new binding.threadsafe_function_ex_call.TSFNWrap(tsfnData => {
    assert(data === tsfnData, "Data in and out of tsfn call do not equal");
  });
  await tsfn.doCall(data);
  await tsfn.release();
}
