// @ts-check
'use strict';
const assert = require('assert');
const buildType = process.config.target_defaults.default_configuration;

// If `true`, this module will re-throw any error caught, allowing the caller to
// handle.
const SHOW_OUTPUT = true;

const print = (isError, newLine, ...what) => {
  if (SHOW_OUTPUT) {
    let target, method;
    target = newLine ? console : process[isError ? 'stderr' : 'stdout'];
    method = target === console ? (isError ? 'error' : 'log') : 'write';
    if (isError) {
      method

    }
    return target[method].apply(target, what);
  }
}

/** @returns {void} */
const log = (...what) => print(false, true, ...what);

/** @returns {void} */
const error = (...what) => print(true, true, ...what);

/** @returns {Promise<number>} */
const write = (...what) => print(false, false, ...what);

/** @returns {Promise<number>} */
// const rewind = () => print(false, false, `\x1b[K`);
const rewind = () => print(false, false, `\x1b[1A`);

const pad = (what, targetLength = 20, padString = ' ', padLeft) => {
  const padder = (pad, str) => {
    if (typeof str === 'undefined')
      return pad;
    if (padLeft) {
      return (pad + str).slice(-pad.length);
    } else {
      return (str + pad).substring(0, pad.length);
    }
  };
  return padder(padString.repeat(targetLength), String(what));
}

/**
 * Test runner helper class. Each static method's name corresponds to the
 * namespace the test as defined in the native addon. Each test specifics are
 * documented on the individual method. The async test handler runs
 * synchronously in the series of all tests so the test **MUST** wait on the
 * finalizer. Otherwise, the test runner will assume the test completed.
 */
class TestRunner {

  static async run(isNoExcept) {
    const binding = require(`../../build/${buildType}/binding${isNoExcept ? '_noexcept' : ''}.node`);
    const runner = new this();
    // Errors thrown are caught by caller, and re-thrown if `BUBBLE_ERRORS` is
    // `true.
    const cmdlineTests = process.argv.length > 2 ? process.argv.slice(2) : null;
    for (const nsName of Object.getOwnPropertyNames(this.prototype)) {
      if (nsName !== 'constructor') {
        const ns = binding.threadsafe_function_ex_basic[nsName];
        let state;
        const setState = (...newState) => { state = newState };
        const toLine = (state) => {
          const [label, time, isNoExcept, nsName] = state;
          const except = () => pad(isNoExcept ? '[noexcept]' : '', 12);
          const timeStr = () => time == null ? '...' : `${time}${typeof time === 'number' ? 'ms' : ''}`;
          return `${pad(nsName, 10)} ${except()}|   ${pad(timeStr(), 8)}| ${pad(label, 15)}`;
        };
        const stateLine = () => toLine(state);
        if (ns && (cmdlineTests == null || cmdlineTests.indexOf(nsName) > -1)) {
          setState('Running test', null, isNoExcept, nsName);
          log(stateLine());

          const start = Date.now();
          await runner[nsName](ns);
          await new Promise(resolve => setTimeout(resolve, 50));
          rewind();
          setState('Finished test', Date.now() - start, isNoExcept, nsName);
          log(stateLine());
        } else {
          setState('Skipping test', '-', isNoExcept, nsName);
          debugger;
          log(stateLine());
        }
      }
    }
  }
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
    await tsfn.release();
  }

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
  async context({ TSFNWrap }) {
    const ctx = {};
    const tsfn = new TSFNWrap(ctx);
    assert(ctx === await tsfn.getContextByCall(), "getContextByCall context not equal");
    assert(ctx === tsfn.getContextFromTsfn(), "getContextFromTsfn context not equal");
    await tsfn.release();
  }

  /**
    * **ONLY ON N-API 5+**. The optional JavaScript function callback feature is
    * not available in N-API <= 4.
    * - Creates a threadsafe function with no JavaScript context or callback.
    * - Makes two calls, waiting for each, and expecting the first to resolve
    *   and the second to reject.
    * - Waits for Node to process the items on the queue prior releasing the
    *   threadsafe function.
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
      await tsfn.release();
    }
  }

  /**
   * A `ThreadSafeFunctionEx<>` can be constructed with default type arguments.
   * - Creates a threadsafe function with no context or callback.
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
    await tsfn.release();
  }

}

async function run() {
  await TestRunner.run(false);
  await TestRunner.run(true);
}


module.exports = run()
  .then(() => { log(`Finished executing tests in .${__filename.replace(process.cwd(), '')}`); })
  .catch((e) => {
    // if (require.main !== module) { throw e; }
    console.error(`Test failed!`, e);
    process.exit(1);
  });

