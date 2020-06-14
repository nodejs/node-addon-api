// @ts-check
'use strict';
const assert = require('assert');

const { TestRunner } = require('../util/TestRunner');


class ExampleTest extends TestRunner {

  async example({ TSFNWrap }) {
    const ctx = {};
    console.log("starting");
    const tsfn = new TSFNWrap(ctx);
    console.log("tsfn is", tsfn);
    console.log("start is", tsfn.start());
    console.log();
    return await tsfn.release();
  }

}

module.exports = new ExampleTest('threadsafe_function_ex_example', __filename).start();
