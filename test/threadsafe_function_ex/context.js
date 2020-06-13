'use strict';

const assert = require('assert');
const buildType = process.config.target_defaults.default_configuration;

module.exports = Promise.all([
  test(require(`../build/${buildType}/binding.node`)),
  test(require(`../build/${buildType}/binding_noexcept.node`))
]);

/**
 * The context provided to the threadsafe function's constructor is accessible
 * on both the threadsafe function's callback as well the threadsafe function
 * itself. This test ensures the context across all three are the same.
 * - Creates a contexted threadsafe function with callback.
 * - The callback forwards the item's data to the given JavaScript function in
 *   the test.
 * - Makes one call, and waits for call to complete.
 * - Asserts the contexts are the same.
 */
async function test(binding) {
  const ctx = {};
  const tsfn = new binding.threadsafe_function_ex_context.TSFNWrap(ctx);
  assert(ctx === await tsfn.getContextByCall(), "getContextByCall context not equal");
  assert(ctx === tsfn.getContextFromTsfn(), "getContextFromTsfn context not equal");
  await tsfn.release();
}
