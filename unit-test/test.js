'use strict';
const path = require('path');
const runChildProcess = require('./spawnTask').runChildProcess;

const executeTests = async function() {
  try {
    const workingDir = path.join(__dirname, '../');
    const relativeBuildPath = path.join('../', 'unit-test');
    const buildPath = path.join(__dirname, './unit-test');
    const envVars = { ...process.env, REL_BUILD_PATH: relativeBuildPath, BUILD_PATH: buildPath};

    console.log('Starting to run tests in ', buildPath, new Date());

    await runChildProcess('test', {cwd: workingDir, env: envVars});

    console.log('Completed running tests', new Date());
  } catch(e) {
    console.log('Error occured running tests', new Date());
  }
}

executeTests();
