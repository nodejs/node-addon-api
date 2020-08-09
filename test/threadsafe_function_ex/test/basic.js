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
    * - Makes one call, waiting for completion. The internal `CallJs` resolves
    *   the call if jsCallback is empty, otherwise rejects.
    */
  async empty({ TSFNWrap }) {
    if (typeof TSFNWrap === 'function') {
      const tsfn = new TSFNWrap();
      await tsfn.call();
      return await tsfn.release();
    }
    return true;
  }

  /**
   * A `ThreadSafeFunctionEx` can be constructed with an existing
   * napi_threadsafe_function.
   * - Creates a native napi_threadsafe_function with no context, using the
   *   jsCallback passed from this test.
   * - Makes six calls:
   *   - Use node-addon-api's `NonBlockingCall` *OR* napi's
   *     `napi_call_threadsafe_function` _cross_
   *     - With data that resolves *OR rejects on CallJs
   *     - With no data that rejects on CallJs
   * - Releases the TSFN.
   */
  async existing({ TSFNWrap }) {

    /**
     * Called by the TSFN's jsCallback below.
     * @type {function|undefined}
     */
    let currentCallback = undefined;

    const tsfn = new TSFNWrap(function () {
      if (typeof currentCallback === 'function') {
        currentCallback.apply(undefined, arguments);
      }
    });
    /**
     * The input argument to `tsfn.call()`: 0-2:
     * ThreadSafeFunctionEx.NonBlockingCall(data) with...
     *    - 0: data, resolve promise in CallJs
     *    - 1: data, reject promise in CallJs
     *    - 2: data = nullptr 3-5: napi_call_threadsafe_function(data,
     *      napi_tsfn_nonblocking) with...
     *    - 3: data, resolve promise in CallJs
     *    - 4: data, reject promise in CallJs
     *    - 5: data = nullptr
     * @type {[0,1,2,3,4,5]}
     */
    const input = [0, 1, 2, 3, 4, 5];

    let caught = false;

    while (input.length) {
      // Perform a call that resolves
      await tsfn.call(input.shift());

      // Perform a call that rejects
      caught = false;
      try {
        await tsfn.call(input.shift());
      } catch (e) {
        caught = true;
      } finally {
        assert(caught, "The rejection was not caught");
      }

      // Perform a call with no data
      caught = false;
      await new Promise((resolve, reject) => {
        currentCallback = () => {
          resolve();
          reject = undefined;
        };
        tsfn.call(input.shift());
        setTimeout(() => {
          if (reject) {
            reject(new Error("tsfn.call() timed out"));
          }
        }, 1000);
      });
    }
    return await tsfn.release();
  }

  /**
   * A `ThreadSafeFunctionEx<>` can be constructed with no template arguments.
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
