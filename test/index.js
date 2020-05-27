'use strict';

// Command line:
// None to launch all tests, otherwise
// $2: name of test
// $3: mode ("worker" or "child")

const { Worker, workerData, isMainThread } = require('worker_threads');

process.config.target_defaults.default_configuration =
  require('fs')
    .readdirSync(require('path').join(__dirname, 'build'))
    .filter((item) => (item === 'Debug' || item === 'Release'))[0];

if (!isMainThread) {
  require('./' + workerData);
  return;
}

if (process.argv.length === 4) {
  if (process.argv[3] === 'child') {
    require('./' + process.argv[2]);
  } else if (process.argv[3] === 'worker') {
    (new Worker(__filename, { workerData: process.argv[2] }))
      .on('exit', (code) => {
        process.exit(code);
      });
  }
  return;
}

// FIXME: We might need a way to load test modules automatically without
// explicit declaration as follows.
let testModules = [
  'addon_data',
  'arraybuffer',
  'asynccontext',
  'asyncprogressqueueworker',
  'asyncprogressworker',
  'asyncworker',
  'asyncworker-nocallback',
  'asyncworker-persistent',
  'basic_types/array',
  'basic_types/boolean',
  'basic_types/number',
  'basic_types/value',
  'bigint',
  'date',
  'buffer',
  'callbackscope',
  'dataview/dataview',
  'dataview/dataview_read_write',
  'error',
  'external',
  'function',
  'handlescope',
  'memory_management',
  'name',
  'object/delete_property',
  'object/finalizer',
  'object/get_property',
  'object/has_own_property',
  'object/has_property',
  'object/object',
  'object/object_deprecated',
  'object/set_property',
  'promise',
  'run_script',
  'threadsafe_function/threadsafe_function_ctx',
  'threadsafe_function/threadsafe_function_existing_tsfn',
  'threadsafe_function/threadsafe_function_ptr',
  'threadsafe_function/threadsafe_function_sum',
  'threadsafe_function/threadsafe_function_unref',
  'threadsafe_function/threadsafe_function',
  'typedarray',
  'typedarray-bigint',
  'objectwrap',
  'objectwrap_constructor_exception',
  'objectwrap-removewrap',
  'objectwrap_multiple_inheritance',
  'objectreference',
  'reference',
  'version_management'
];

const napiVersion = Number(process.versions.napi)

if (napiVersion < 3) {
  testModules.splice(testModules.indexOf('callbackscope'), 1);
  testModules.splice(testModules.indexOf('version_management'), 1);
}

if (napiVersion < 4) {
  testModules.splice(testModules.indexOf('asyncprogressqueueworker'), 1);
  testModules.splice(testModules.indexOf('asyncprogressworker'), 1);
  testModules.splice(testModules.indexOf('threadsafe_function/threadsafe_function_ctx'), 1);
  testModules.splice(testModules.indexOf('threadsafe_function/threadsafe_function_existing_tsfn'), 1);
  testModules.splice(testModules.indexOf('threadsafe_function/threadsafe_function_ptr'), 1);
  testModules.splice(testModules.indexOf('threadsafe_function/threadsafe_function_sum'), 1);
  testModules.splice(testModules.indexOf('threadsafe_function/threadsafe_function_unref'), 1);
  testModules.splice(testModules.indexOf('threadsafe_function/threadsafe_function'), 1);
}

if (napiVersion < 5) {
  testModules.splice(testModules.indexOf('date'), 1);
}

if (napiVersion < 6) {
  testModules.splice(testModules.indexOf('bigint'), 1);
  testModules.splice(testModules.indexOf('typedarray-bigint'), 1);
  testModules.splice(testModules.indexOf('addon_data'), 1);
}

console.log(`Testing with N-API Version '${napiVersion}'.`);
console.log('Starting test suite\n');

function runOneChild(name, mode) {
  console.log(`Running test '${name}' as ${mode}`);
  const child = require('./napi_child').spawnSync(process.execPath, [
    __filename, name, mode
  ], { stdio: 'inherit' });
  if (child.signal) {
    console.error(`Test ${name} run as ${mode} aborted with ${child.signal}`);
    if (!process.exitCode) {
      process.exitCode = 1;
    }
  }
  if (!process.exitCode && child.status !== 0) {
    process.exitCode = child.status;
  }
}

testModules.forEach((name) => {
  runOneChild(name, 'child');
  runOneChild(name, 'worker');
});

process.exit(process.exitCode);
