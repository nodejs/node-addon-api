'use strict';

const assert = require('assert');

const buildType = process.config.target_defaults.default_configuration;

module.exports = Promise.all([
  test(require(`../build/${buildType}/binding.node`)),
  test(require(`../build/${buildType}/binding_noexcept.node`))
]);

async function test(binding) {
  const testCall = binding.threadsafe_function_existing_tsfn.testCall;
  let lastResult;
  
  const funct = arg0 => {
    lastResult = arg0;
  }
  
  await testCall({ blocking: true,  data: true  }, funct);
  assert(typeof lastResult === "number");
  await testCall({ blocking: true,  data: false }, funct);
  assert(typeof lastResult === "undefined");
  await testCall({ blocking: false, data: true  }, funct);
  assert(typeof lastResult === "number");
  await testCall({ blocking: false, data: false }, funct);
  assert(typeof lastResult === "undefined");
}
