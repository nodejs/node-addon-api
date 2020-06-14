// @ts-check
'use strict';
const assert = require('assert');

const { TestRunner } = require('../util/TestRunner');


class ExampleTest extends TestRunner {

  async example({ TSFNWrap }) {
    const ctx = {};
    const tsfn = new TSFNWrap(ctx);
    const run = async (i) => { 
      const result = tsfn.start({threads:[1]});
      await result;
      return await tsfn.release();
    };
    const results = [ await run(1) ];
    results.push( await run(2) );
    return results;
    // return await Promise.all( [ run(1), run(2) ] );
    // await run(2);
  }

}

module.exports = new ExampleTest('threadsafe_function_ex_example', __filename).start();
