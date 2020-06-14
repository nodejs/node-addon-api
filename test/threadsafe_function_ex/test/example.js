'use strict';

/**
 * This test is programmatically represents the example shown in
 * `doc/threadsafe_function_ex.md`
 */

const assert = require('assert');
const buildType = 'Debug'; process.config.target_defaults.default_configuration;

const isCI = require.main !== module;
const print = (isError, ...what) => isCI ? () => {} : console[isError ? 'error' : 'log'].apply(console.log, what);
const log = (...what) => print(false, ...what);
const error = (...what) => print(true, ...what);

module.exports = Promise.all([
  // test(require(`../build/${buildType}/binding_noexcept.node`)),
  isCI ? undefined : test(require(`../build/${buildType}/binding_noexcept.node`))
]).catch(e => {
  console.error('Error', e);
});

async function test(binding) {
  try {
    const tsfn = new binding.threadsafe_function_ex_example.TSFNWrap(true);
  await tsfn.start({
      threads: ['f',5,5,5],
      callback: ()=>{}
  });
  await tsfn.release();
} catch (e) { 
  error(e);
}
}


if (!isCI) {
  console.log(module.exports);
  module.exports.then(() => {
    log('tea');
  }).catch((e) => {
    error('Error!', e);
  });
}
