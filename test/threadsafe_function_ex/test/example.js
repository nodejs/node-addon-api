// @ts-check
'use strict';
const assert = require('assert');

const { TestRunner } = require('../util/TestRunner');


class ExampleTest extends TestRunner {

  async example({ TSFNWrap }) {
    const ctx = {};
    const tsfn = new TSFNWrap(ctx);
    assert(ctx === await tsfn.call(), "getContextByCall context not equal");
    assert(ctx === tsfn.getContext(), "getContextFromTsfn context not equal");
    return await tsfn.release();
  }

}

module.exports = new ExampleTest('threadsafe_function_ex_example', __filename).start();
