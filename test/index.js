'use strict';

const majorNodeVersion = process.versions.node.split('.')[0];

if (typeof global.gc !== 'function') {
  // Construct the correct (version-dependent) command-line args.
  let args = ['--expose-gc'];
  const majorV8Version = process.versions.v8.split('.')[0];
  if (majorV8Version < 9) {
    args.push('--no-concurrent-array-buffer-freeing');
  }
  if (majorNodeVersion >= 14) {
    args.push('--no-concurrent-array-buffer-sweeping');
  }
  args.push(__filename);

  const child = require('./napi_child').spawnSync(process.argv[0], args, {
    stdio: 'inherit',
  });

  if (child.signal) {
    console.error(`Tests aborted with ${child.signal}`);
    process.exitCode = 1;
  } else {
    process.exitCode = child.status;
  }
  process.exit(process.exitCode);
}

const fs = require('fs');
const path = require('path');
process.env.filter = require('../unit-test/matchModules').matchWildCards(process.env.filter);

let testModules = [];

const filterCondition = process.env.filter || '';
const filterConditionFiles = filterCondition.split(' ').length ?  filterCondition.split(' ') : [filterCondition];


function checkFilterCondition(fileName, parsedFilepath) {
  let result = false;
  
  if (!filterConditionFiles.length) result = true;
  if (filterConditionFiles.includes(parsedFilepath)) result = true;
  if (filterConditionFiles.includes(fileName)) result = true;
  return result;
}

// TODO(RaisinTen): Update this when the test filenames
// are changed into test_*.js.
function loadTestModules(currentDirectory = __dirname, pre = '') {
  fs.readdirSync(currentDirectory).forEach((file) => {
    if (currentDirectory === __dirname && (
          file === 'binding.cc' ||
          file === 'binding.gyp' ||
          file === 'build' ||
          file === 'common' ||
          file === 'napi_child.js' ||
          file === 'testUtil.js' ||
          file === 'thunking_manual.cc' ||
          file === 'thunking_manual.js' ||
          file === 'index.js' ||
          file[0] === '.')) {
      return;
    }
    const absoluteFilepath = path.join(currentDirectory, file);
    const parsedFilepath = path.parse(file);
    const parsedPath = path.parse(currentDirectory);

    if (fs.statSync(absoluteFilepath).isDirectory()) {
      if (fs.existsSync(absoluteFilepath + '/index.js')) {
        if (checkFilterCondition(parsedFilepath.name, parsedPath.base)) {
          testModules.push(pre + file);
        }
      } else {
        loadTestModules(absoluteFilepath, pre + file + '/');
      }
    } else {
      if (parsedFilepath.ext === '.js' && checkFilterCondition(parsedFilepath.name, parsedPath.base)) {
        testModules.push(pre + parsedFilepath.name);
      }
    }
  });
}

loadTestModules();

process.config.target_defaults.default_configuration =
  fs
    .readdirSync(path.join(__dirname, process.env.REL_BUILD_PATH, 'build'))
    .filter((item) => (item === 'Debug' || item === 'Release'))[0];

let napiVersion = Number(process.versions.napi);
if (process.env.NAPI_VERSION) {
  // we need this so that we don't try run tests that rely
  // on methods that are not available in the NAPI_VERSION
  // specified
  napiVersion = process.env.NAPI_VERSION;
}
console.log('napiVersion:' + napiVersion);

if (napiVersion < 3) {
  testModules.splice(testModules.indexOf('env_cleanup'), 1);
  testModules.splice(testModules.indexOf('callbackscope'), 1);
  testModules.splice(testModules.indexOf('version_management'), 1);
}

if (napiVersion < 4 && !filterConditionFiles.length) {
  testModules.splice(testModules.indexOf('asyncprogressqueueworker'), 1);
  testModules.splice(testModules.indexOf('asyncprogressworker'), 1);
  testModules.splice(testModules.indexOf('threadsafe_function/threadsafe_function_ctx'), 1);
  testModules.splice(testModules.indexOf('threadsafe_function/threadsafe_function_existing_tsfn'), 1);
  testModules.splice(testModules.indexOf('threadsafe_function/threadsafe_function_ptr'), 1);
  testModules.splice(testModules.indexOf('threadsafe_function/threadsafe_function_sum'), 1);
  testModules.splice(testModules.indexOf('threadsafe_function/threadsafe_function_unref'), 1);
  testModules.splice(testModules.indexOf('threadsafe_function/threadsafe_function'), 1);
}

if (napiVersion < 5 && !filterConditionFiles.length) {
  testModules.splice(testModules.indexOf('date'), 1);
}

if (napiVersion < 6 && !filterConditionFiles.length) {
  testModules.splice(testModules.indexOf('addon'), 1);
  testModules.splice(testModules.indexOf('addon_data'), 1);
  testModules.splice(testModules.indexOf('bigint'), 1);
  testModules.splice(testModules.indexOf('typedarray-bigint'), 1);
}

if (majorNodeVersion < 12 && !filterConditionFiles.length) {
  testModules.splice(testModules.indexOf('objectwrap_worker_thread'), 1);
  testModules.splice(testModules.indexOf('error_terminating_environment'), 1);
}

if (napiVersion < 8 && !filterConditionFiles.length) {
  testModules.splice(testModules.indexOf('object/object_freeze_seal'), 1);
}

(async function() {
  console.log(`Testing with Node-API Version '${napiVersion}'.`);

  console.log('Starting test suite\n', testModules);

  // Requiring each module runs tests in the module.
  for (const name of testModules) {
    console.log(`Running test '${name}'`);
    await require('./' + name);
  };

  console.log('\nAll tests passed!');
})().catch((error) => {
  console.log(error);
  process.exit(1);
});
