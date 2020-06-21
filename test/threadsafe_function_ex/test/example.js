// @ts-check
'use strict';
const assert = require('assert');
const { TestRunner } = require('../util/TestRunner');

/**
 * @typedef {(threadId: number, callId: number, logCall: boolean, value: number, callDelay:
 * number)=>number|Promise<number>} TSFNCallback
 */

/**
 * @typedef {Object} ThreadOptions
 * @property {number} calls
 * @property {number} callDelay
 * @property {number} threadDelay
 */

/**
 * The options when starting the addon's TSFN.
 * @typedef {Object} StartOptions
 * @property {ThreadOptions[]} threads
 * @property {boolean} [logCall] If `true`, log messages via `console.log`.
 * @property {boolean} [logThread] If `true`, log messages via `std::cout`.
 * @property {number} [acquireFactor] Acquire a new set of \`acquireFactor\`
 * call threads. `NAPI_CPP_EXCEPTIONS`, allowing errors to be caught as
 * exceptions.
 * @property {TSFNCallback} callback The callback provided to the threadsafe
 * function.
 * @property {[number,number]} [callbackError] Tuple of `[threadId, callId]` to
 * cause an error on
*/

/**
 * Returns test options.
 * @type {() => { options: StartOptions, calls: { aggregate: number } }}
 */
const getTestDetails = () => {
  const TEST_CALLS = [1, 2, 3, 4, 5];
  const TEST_ACQUIRE = 1;
  const TEST_CALL_DELAY = [400, 200, 100, 50, 0]
  const TEST_THREAD_DELAY = TEST_CALL_DELAY.map(_ => _);
  const TEST_LOG_CALL = false;
  const TEST_LOG_THREAD = false;
  const TEST_NO_CALLBACK = false;

  /** @type {[number, number] | undefined} [threadId, callId] */
  const TEST_CALLBACK_ERROR = undefined;

  // Set options as defaults
  let testCalls = TEST_CALLS;
  let testAcquire = TEST_ACQUIRE;
  let testCallDelay = TEST_CALL_DELAY;
  let testThreadDelay = TEST_THREAD_DELAY;
  let testLogCall = TEST_LOG_CALL;
  let testLogThread = TEST_LOG_THREAD;
  let testNoCallback = TEST_NO_CALLBACK;
  let testCallbackError = TEST_CALLBACK_ERROR;

  let args = process.argv.slice(2);
  let arg;

  const showHelp = () => {
    console.log(
      `
Usage: ${process.argv0} .${process.argv[1].replace(process.cwd(), '')} [options]

      -c, --calls <calls>                   The number of calls each thread should make (number[]).
      -a, --acquire [factor]                Acquire a new set of \`factor\` call threads, using the
                                            same \`calls\` definition.
      -d, --call-delay <call-delays>        The delay on callback resolution that each thread should
                                            have (number[]). This is achieved via a delayed Promise
                                            resolution in the JavaScript callback provided to the
                                            TSFN. Using large delays here will cause all threads to
                                            bottle-neck.
      -D, --thread-delay <thread-delays>    The delay that each thread should have prior to making a
                                            call (number[]). Using large delays here will cause the
                                            individual thread to bottle-neck.
      -l, --log-call                        Display console.log-based logging messages.
      -L, --log-thread                      Display std::cout-based logging messages.
      -n, --no-callback                     Do not use a JavaScript callback.
      -e, --callback-error [thread[.call]]  Cause an error to occur in the JavaScript callback for
                                            the given thread's call (if provided; first thread's
                                            first call otherwise).

  When not provided:
      - <calls> defaults to [${TEST_CALLS}]
      - [factor] defaults to ${TEST_ACQUIRE}
      - <call-delays> defaults to [${TEST_CALL_DELAY}]
      - <thread-delays> defaults to [${TEST_THREAD_DELAY}]


Examples:

      -c [1,2,3] -l -L

          Creates three threads that makes one, two, and three calls each, respectively.

      -c [5,5] -d [5000,5000] -D [0,0] -l -L

          Creates two threads that make five calls each. In this scenario, the threads will be
          blocked primarily on waiting for the callback to resolve, as each thread's call takes
          5000 milliseconds.
`
    );
    return undefined;
  };

  while ((arg = args.shift())) {
    switch (arg) {
      case "-h":
      case "--help":
        return showHelp();

      case "--calls":
      case "-c":
        try {
          testCalls = JSON.parse(args.shift());
        } catch (ex) { /* ignore */ }
        break;

      case "--acquire":
      case "-a":
        testAcquire = parseInt(args[0]);
        if (!isNaN(testAcquire)) {
          args.shift();
        } else {
          testAcquire = TEST_ACQUIRE;
        }
        break;

      case "--call-delay":
      case "-d":
        try {
          testCallDelay = JSON.parse(args.shift());
        } catch (ex) { /* ignore */ }
        break;

      case "--thread-delay":
      case "-D":
        try {
          testThreadDelay = JSON.parse(args.shift());
        } catch (ex) { /* ignore */ }
        break;

      case "--log-call":
      case "-l":
        testLogCall = true;
        break;

      case "--log-thread":
      case "-L":
        testLogThread = true;
        break;

      case "--no-callback":
      case "-n":
        testNoCallback = true;
        break;

      case "-e":
      case "--callback-error":
        try {
          if (!args[0].startsWith("-")) {
            const split = args.shift().split(/\./);
            testCallbackError = [parseInt(split[0], 10) || 0, parseInt(split[1], 10) || 0];
          }
        }
        catch (ex) { /*ignore*/ }
        finally {
          if (!testCallbackError) {
            testCallbackError = [0, 0];
          }
        }
        break;

      default:
        console.error("Unknown option:", arg);
        return showHelp();
    }
  }

  if (testCallbackError && testNoCallback) {
    console.error("--error cannot be used in conjunction with --no-callback");
    return undefined;
  }

  testCalls = Array.isArray(testCalls) ? testCalls : TEST_CALLS;

  const calls = { aggregate: testNoCallback ? null : 0 };

  /**
     * The JavaScript callback provided to our TSFN.
     * @callback TSFNCallback
     * @param {number} threadId Thread Id
     * @param {number} callId Call Id
     * @param {boolean} logCall If true, log messages to console regarding this
     * call.
     * @param {number} base The input as calculated from CallJs
     * @param {number} callDelay If `> 0`, return a `Promise` that resolves with
     * `value` after `callDelay` milliseconds. Otherwise, return a `number`
     * whose value is `value`.
     */

  /** @type {undefined | TSFNCallback} */
  const callback = testNoCallback ? undefined : (threadId, callId, logCall, base, callDelay) => {
    // Calculate the result value as `base * base`.
    const value = base * base;

    // Add the value to our call aggregate
    calls.aggregate += value;

    if (testCallbackError !== undefined && testCallbackError[0] === threadId && testCallbackError[1] === callId) {
      return new Error(`Test throw error for ${threadId}.${callId}`);
    }

    // If `callDelay > 0`, then return a Promise that resolves with `value` after
    // `callDelay` milliseconds.
    if (callDelay > 0) {
      // Logging messages.
      if (logCall) {
        console.log(`[Thread ${threadId}] [Callback] [Call ${callId}] Receive request: base = ${base}, delay = ${callDelay}ms`);
      }

      const start = Date.now();

      return new Promise(resolve => setTimeout(() => {
        // Logging messages.
        if (logCall) {
          console.log(`[Thread ${threadId}] [Callback] [Call ${callId}] Answer request: base = ${base}, value = ${value} after ${Date.now() - start}ms`);
        }
        resolve(value);
      }, callDelay));
    }

    // Otherwise, return a `number` whose value is `value`.
    else {
      // Logging messages.
      if (logCall) {
        console.log(`[Thread ${threadId}] [Callback] [Call ${callId}] Receive, answer request: base = ${base}, value = ${value}`);
      }
      return value;
    }
  };

  return {
    options: {
      // Construct `ThreadOption[] threads` from `number[] testCalls`
      threads: testCalls.map((callCount, index) => ({
        calls: callCount,
        callDelay: testCallDelay !== null && typeof testCallDelay[index] === 'number' ? testCallDelay[index] : 0,
        threadDelay: testThreadDelay !== null && typeof testThreadDelay[index] === 'number' ? testThreadDelay[index] : 0,
      })),
      logCall: testLogCall,
      logThread: testLogThread,
      acquireFactor: testAcquire,
      callback,
      callbackError: testCallbackError
    },
    calls
  };
}

class ExampleTest extends TestRunner {

  async example({ TSFNWrap }) {

    /**
     * @typedef {Object} TSFNWrap
     * @property {(opts: StartOptions) => number} start Start the TSFN. Returns
     * the number of threads started.
     * @property {() => Promise<boolean>} release Release the TSFN.
     * @property {() => [number, number]} callCount Returns the call aggregates
     * as counted by the TSFN:
     * - `[0]`: The sum of the number of calls by each thread.
     * - `[1]`: The sum of the `value`s returned by each call by each thread.
     * @property {(threads: ThreadOptions[]) => number} acquire
     */

    /** @type {TSFNWrap} */
    const tsfn = new TSFNWrap();

    const testDetails = getTestDetails();
    if (testDetails === undefined) {
      throw new Error("No test details");
    }
    const { options } = testDetails;
    const { acquireFactor, threads, callback, callbackError } = options;

    /**
     * Start the TSFN with the given options. This will create the TSFN with initial
     * thread count of `threads.length + 1` (+1 due to the Node thread using the TSFN)
     */
    const startedActual = tsfn.start(options);

    /**
     * The initial
     */
    const threadsPerSet = threads.length;

    /**
     * Calculate the expected results. Create a new list of thread options by
     * concatinating `threads` by `acquireFactor` times.
     */
    const startedThreads = [...new Array(acquireFactor)].map(_ => threads).reduce((p, c) => p.concat(c), []);

    const expected = startedThreads.reduce((p, threadCallCount, threadId) => (
      ++threadId,
      ++p.threadCount,
      p.callCount += threadCallCount.calls,
      p.aggregate += threadCallCount.calls * threadId ** 2,
      p.callbackAggregate = p.callbackAggregate === null ? null : p.aggregate,
      p
    ), { threadCount: 0, callCount: 0, aggregate: 0, callbackAggregate: callback ? 0 : null });

    if (typeof startedActual === 'number') {
      const { threadCount, callCount, aggregate, callbackAggregate } = expected;
      assert(startedActual === threadsPerSet, `The number of threads when starting the TSFN do not match: actual = ${startedActual}, expected = ${threadsPerSet}`)
      for (let i = 1; i < acquireFactor; ++i) {
        const acquiredActual = tsfn.acquire(threads);
        assert(acquiredActual === threadsPerSet, `The number of threads when acquiring a new set of threads do not match: actual = ${acquiredActual}, expected = ${threadsPerSet}`)
      }
      const released = await tsfn.release();
      const [callCountActual, aggregateActual] = tsfn.callCount();
      const { calls } = testDetails;
      const { aggregate: actualCallAggregate } = calls;
      if (!callbackError) {
        assert(callCountActual === callCount, `The number of calls do not match: actual = ${callCountActual}, expected = ${callCount}`);
        assert(aggregateActual === aggregate, `The aggregate of calls do not match: actual = ${aggregateActual}, expected = ${aggregate}`);
        assert(actualCallAggregate === callbackAggregate, `The number aggregated by the JavaScript callback and the thread calculated aggregate do not match: actual ${actualCallAggregate}, expected = ${aggregate}`)
        return { released, ...expected };
      }
      // The test runner erases the last line, so write an empty line.
      if (options.logCall) { console.log(); }
      return released;
    } else {
      throw new Error('The TSFN failed to start');
    }
  }

}

module.exports = new ExampleTest('threadsafe_function_ex_example', __filename).start();
