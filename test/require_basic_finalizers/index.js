'use strict';

const { promisify } = require('util');
const exec = promisify(require('child_process').exec);
const { copy, remove } = require('fs-extra');
const path = require('path');
const assert = require('assert');

async function test () {
  const addon = 'require-basic-finalizers';
  const ADDON_FOLDER = path.join(__dirname, 'addons', addon);

  await remove(ADDON_FOLDER);
  await copy(path.join(__dirname, 'tpl'), ADDON_FOLDER);

  console.log('   >Building addon');

  // Fail when NODE_ADDON_API_REQUIRE_BASIC_FINALIZERS is enabled
  await assert.rejects(exec('npm --require-basic-finalizers install', {
    cwd: ADDON_FOLDER
  }), 'Addon unexpectedly compiled successfully');

  // Succeed when NODE_ADDON_API_REQUIRE_BASIC_FINALIZERS is not enabled
  return assert.doesNotReject(exec('npm install', {
    cwd: ADDON_FOLDER
  }));
}

module.exports = (function () {
  // This test will only run under an experimental version test.
  const isExperimental = Number(process.env.NAPI_VERSION) === 2147483647;

  if (isExperimental) {
    return test();
  } else {
    console.log('   >Skipped (non-experimental test run)');
  }
})();
