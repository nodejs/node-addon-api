// @ts-check
'use strict';
const assert = require('assert');

const { TestRunner } = require('../util/TestRunner');

/**
 * A "basic" test spec. This spec does NOT use threads, and is primarily used to
 * verify the API.
 */
class BasicTest extends TestRunner {
  /**
   * This test ensures the data sent to the NonBlockingCall and the data
   * received in the JavaScript callback are the same.
   * - Creates a contexted threadsafe function with callback.
   * - Makes one call, and waits for call to complete.
   * - The callback forwards the item's data to the given JavaScript function in
   *   the test.
   * - Asserts the data is the same.
   */
  async call({ TSFNWrap }) {
    const data = {};
    const tsfn = new TSFNWrap(tsfnData => {
      assert(data === tsfnData, "Data in and out of tsfn call do not equal");
    });
    await tsfn.call(data);
    return await tsfn.release();
  }

  /**
   * The context provided to the threadsafe function's constructor is accessible
   * on both (A) the threadsafe function's callback as well as (B) the
   * threadsafe function itself. This test ensures the context across all three
   * are the same.
   * - Creates a contexted threadsafe function with callback.
   * - The callback forwards the item's data to the given JavaScript function in
   *   the test.
   * - Asserts the contexts are the same as the context passed during threadsafe
   *   function construction in two places:
   *    - (A) Makes one call, and waits for call to complete. 
   *    - (B) Asserts that the context returns from the API's `GetContext()`
   */
  async context({ TSFNWrap }) {
    const ctx = {};
    const tsfn = new TSFNWrap(ctx);
    assert(ctx === await tsfn.call(), "getContextByCall context not equal");
    assert(ctx === tsfn.getContext(), "getContextFromTsfn context not equal");
    return await tsfn.release();
  }

  /**
    * **ONLY ON N-API 5+**. The optional JavaScript function callback feature is
    * not available in N-API <= 4. This test creates uses a threadsafe function
    * that handles all of its JavaScript processing on the callJs instead of the
    * callback.
    * - Creates a threadsafe function with no JavaScript context or callback.
    * - Makes two calls, waiting for each, and expecting the first to resolve
    *   and the second to reject.
    */
  async empty({ TSFNWrap }) {
    debugger;
    if (typeof TSFNWrap === 'function') {
      const tsfn = new TSFNWrap();
      await tsfn.call(false /* reject */);
      let caught = false;
      try {
        await tsfn.call(true /* reject */);
      } catch (ex) {
        caught = true;
      }

      assert.ok(caught, 'The promise rejection was not caught');
      return await tsfn.release();
    }
    return true;
  }

  /**
   * A `ThreadSafeFunctionEx<>` can be constructed with no type arguments.
   * - Creates a threadsafe function with no context or callback or callJs.
   * - The node-addon-api 'no callback' feature is implemented by passing either
   *   a no-op `Function` on N-API 4 or `std::nullptr` on N-API 5+ to the
   *   underlying `napi_create_threadsafe_function` call.
   * - Makes one call, releases, then waits for finalization.
   * - Inherently ignores the state of the item once it has been added to the
   *   queue. Since there are no callbacks or context, it is impossible to
   *   capture the state.
   */
  async simple({ TSFNWrap }) {
    const tsfn = new TSFNWrap();
    tsfn.call();
    return await tsfn.release();
  }

}

module.exports = new BasicTest('threadsafe_function_ex_basic', __filename).start();
