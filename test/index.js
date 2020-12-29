'use strict';

process.config.target_defaults.default_configuration =
  require('fs')
    .readdirSync(require('path').join(__dirname, 'build'))
    .filter((item) => (item === 'Debug' || item === 'Release'))[0];

// FIXME: We might need a way to load test modules automatically without
// explicit declaration as follows.
let testModules = [
  'addon_build',
  'addon',
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
  'typed_threadsafe_function/typed_threadsafe_function_ctx',
  'typed_threadsafe_function/typed_threadsafe_function_existing_tsfn',
  'typed_threadsafe_function/typed_threadsafe_function_ptr',
  'typed_threadsafe_function/typed_threadsafe_function_sum',
  'typed_threadsafe_function/typed_threadsafe_function_unref',
  'typed_threadsafe_function/typed_threadsafe_function',
  'typedarray',
  'typedarray-bigint',
  'objectwrap',
  'objectwrap_constructor_exception',
  'objectwrap_ref',
  'objectwrap-removewrap',
  'objectwrap_multiple_inheritance',
  'objectwrap_worker_thread',
  'objectreference',
  'reference',
  'version_management'
];

let napiVersion = Number(process.versions.napi)
if (process.env.NAPI_VERSION) {
  // we need this so that we don't try run tests that rely
  // on methods that are not available in the NAPI_VERSION
  // specified
  napiVersion = process.env.NAPI_VERSION;
}
console.log('napiVersion:' + napiVersion);

const majorNodeVersion = process.versions.node.split('.')[0]

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
  testModules.splice(testModules.indexOf('addon'), 1);
  testModules.splice(testModules.indexOf('addon_data'), 1);
  testModules.splice(testModules.indexOf('bigint'), 1);
  testModules.splice(testModules.indexOf('typedarray-bigint'), 1);
}

if (majorNodeVersion < 12) {
  testModules.splice(testModules.indexOf('objectwrap_worker_thread'), 1);
}

if (typeof global.gc === 'function') {
  (async function() {
  console.log(`Testing with N-API Version '${napiVersion}'.`);

  console.log('Starting test suite\n');

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
} else {
  // Construct the correct (version-dependent) command-line args.
  let args = ['--expose-gc', '--no-concurrent-array-buffer-freeing'];
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
