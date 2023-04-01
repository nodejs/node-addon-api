'use strict';

const { Worker, isMainThread, /* parentPort, */workerData } = require('node:worker_threads');
const common = require('../common');

// Main test body
async function test (bindingPath) {
  if (!bindingPath.match(/binding[.]node$/)) {
    return;
  }
  console.log(bindingPath);
  const worker = new Worker(__filename, {
    workerData: bindingPath
  });
  worker.on('message', () => {
    setImmediate(() => {
      worker.terminate();
    });
  });
  worker.on('exit', common.mustCall(() => {}, 1));
}

if (isMainThread) {
  module.exports = common.runTestWithBindingPath(test);
} else {
  const ThreadSafeFunctionObjectWrap = require(workerData).threadsafe_function_objectwrap;
  new ThreadSafeFunctionObjectWrap(() => {
    console.log('Called');
  });
  while (true);
}
