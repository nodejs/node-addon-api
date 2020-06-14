// @ts-check
'use strict';
const assert = require('assert');

const { TestRunner } = require('../util/TestRunner');


class ExampleTest extends TestRunner {

  async example({ TSFNWrap }) {
    const tsfn = new TSFNWrap();

    const threads = [1]; //, 2, 2, 5, 12];
    const started = await tsfn.start({ threads, logThread: true });

    /**
     * Calculate the expected results.
     */
    const expected = threads.reduce((p, threadCallCount, threadId) => (
      ++threadId,
      p[0] += threadCallCount,
      p[1] += threadCallCount * threadId ** 2,
      p
    ), [0, 0]);

    if (started) {
      const released = await tsfn.release();
      const [callCountActual, aggregateActual] = tsfn.callCount();
      const [callCountExpected, aggregateExpected] = expected;
      assert(callCountActual == callCountExpected, `The number of calls do not match: actual = ${callCountActual}, expected = ${callCountExpected}`);
      assert(aggregateActual == aggregateExpected, `The aggregate of calls do not match: actual = ${aggregateActual}, expected = ${aggregateExpected}`);
      return expected;
    } else {
      throw new Error('The TSFN failed to start');
    }
  }

}

module.exports = new ExampleTest('threadsafe_function_ex_example', __filename).start();
