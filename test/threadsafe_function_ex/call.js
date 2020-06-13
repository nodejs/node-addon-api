'use strict';

const assert = require('assert');
const buildType = process.config.target_defaults.default_configuration;

module.exports = Promise.all([
  test(require(`../build/${buildType}/binding.node`)),
  test(require(`../build/${buildType}/binding_noexcept.node`))
]);

/**
 * This test ensures the data sent to the NonBlockingCall and the data received
 * in the JavaScript callback are the same.
 * - Creates a contexted threadsafe function with callback.
 * - Makes one call, and waits for call to complete.
 * - The callback forwards the item's data to the given JavaScript function in
 *   the test.
 * - Asserts the data is the same.
 */
async function test(binding) {
  const data = {};
  const tsfn = new binding.threadsafe_function_ex_call.TSFNWrap(tsfnData => {
    assert(data === tsfnData, "Data in and out of tsfn call do not equal");
  });
  await tsfn.doCall(data);
  await tsfn.release();
}
