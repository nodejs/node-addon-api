'use strict';

const assert = require('assert');
const buildType = process.config.target_defaults.default_configuration;

module.exports = Promise.all([
  test(require(`../build/${buildType}/binding.node`)),
  test(require(`../build/${buildType}/binding_noexcept.node`))
].reduce((p, c) => p.concat(c)), []);

function test(binding) {
  return [
    // testSimple(binding),
    testEmpty(binding)
  ];
}

/**
 * A simple, fire-and-forget test.
 * - Creates a threadsafe function with no context or callback.
 * - The node-addon-api 'no callback' feature is implemented by passing either a
 *   no-op `Function` on N-API 4 or `std::nullptr` on N-API 5+ to the underlying
 *   `napi_create_threadsafe_function` call.
 * - Makes one call, releases, then waits for finalization.
 * - Inherently ignores the state of the item once it has been added to the
 *   queue. Since there are no callbacks or context, it is impossible to capture
 *   the state.
 */
async function testSimple(binding) {
  const tsfn = new binding.threadsafe_function_ex_simple.TSFNWrap();
  tsfn.call();
  await tsfn.release();
}

/**
 * **ONLY ON N-API 5+**. The optional JavaScript function callback feature is
 * not available in N-API <= 4.
 * - Creates a threadsafe function with no JavaScript context or callback.
 * - Makes two calls, expecting the first to resolve and the second to reject.
 * - Waits for Node to process the items on the queue prior releasing the
 *   threadsafe function.
 */
async function testEmpty(binding) {
  const { EmptyTSFNWrap } = binding.threadsafe_function_ex_simple;

  if (typeof EmptyTSFNWrap === 'function') {
    const tsfn = new binding.threadsafe_function_ex_simple.EmptyTSFNWrap();
    await tsfn.call(false /* reject */);
    let caught = false;
    try {
      await tsfn.call(true /* reject */);
    } catch (ex) {
      caught = true;
    }

    assert.ok(caught, 'The promise rejection was not caught');
    await tsfn.release();
  }
}
