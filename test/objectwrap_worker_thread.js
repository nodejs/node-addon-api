'use strict';
const path = require('path');
const { Worker, isMainThread, workerData } = require('worker_threads');

module.exports = require('./common').runTestWithBuildType(test);

async function test (buildType) {
  if (isMainThread) {
    const buildType = process.config.target_defaults.default_configuration;
    const worker = new Worker(__filename, { workerData: buildType });
    return new Promise((resolve, reject) => {
      worker.on('exit', () => {
        resolve();
      });
    }, () => {});
  } else {
    await require(path.join(__dirname, 'objectwrap.js'));
  }
}
