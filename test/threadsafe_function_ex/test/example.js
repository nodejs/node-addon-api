// @ts-check
'use strict';
const assert = require('assert');

const { TestRunner } = require('../util/TestRunner');


class ExampleTest extends TestRunner {

  async example({ TSFNWrap }) {
    const tsfn = new TSFNWrap();

    const threads = [1, 2, 3, 4, 5];
    let callAggregate = 0;
    const startedActual = await tsfn.start({
      threads,
      logThread: false,
      logCall: false,

      callback: (_ /*threadId*/, valueFromCallJs) => {
        callAggregate += valueFromCallJs;
      }
    });

    /**
     * Calculate the expected results.
     */
    const expected = threads.reduce((p, threadCallCount, threadId) => (
      ++threadId,
      ++p.threadCount,
      p.callCount += threadCallCount,
      p.aggregate += threadCallCount * threadId ** 2,
      p
    ), { threadCount: 0, callCount: 0, aggregate: 0 });

    if (typeof startedActual === 'number') {
      const released = await tsfn.release();
      const [callCountActual, aggregateActual] = tsfn.callCount();
      const { threadCount, callCount, aggregate } = expected;
      assert(startedActual === threadCount, `The number of threads started do not match: actual = ${startedActual}, expected = ${threadCount}`)
      assert(callCountActual === callCount, `The number of calls do not match: actual = ${callCountActual}, expected = ${callCount}`);
      assert(aggregateActual === aggregate, `The aggregate of calls do not match: actual = ${aggregateActual}, expected = ${aggregate}`);
      assert(aggregate === callAggregate, `The number aggregated by the JavaScript callback and the thread calculated aggregate do not match: actual ${aggregate}, expected = ${callAggregate}`)
      return { released, ...expected, callAggregate };
    } else {
      throw new Error('The TSFN failed to start');
    }
  }

}

module.exports = new ExampleTest('threadsafe_function_ex_example', __filename).start();
