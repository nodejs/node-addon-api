'use strict';

process.config.target_defaults.default_configuration =
  require('fs')
    .readdirSync(require('path').join(__dirname, 'build'))
    .filter((item) => (item === 'Debug' || item === 'Release'))[0];

// FIXME: We might need a way to load test modules automatically without
// explicit declaration as follows.
let testModules = [
  'arraybuffer',
  'asynccontext',
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
  'object/get_property',
  'object/has_own_property',
  'object/has_property',
  'object/object',
  'object/object_deprecated',
  'object/set_property',
  'promise',
  'threadsafe_function/threadsafe_function_ptr',
  'threadsafe_function/threadsafe_function',
  'typedarray',
  'typedarray-bigint',
  'objectwrap',
  'objectreference',
  'version_management'
];

if ((process.env.npm_config_NAPI_VERSION !== undefined) &&
    (process.env.npm_config_NAPI_VERSION < 50000)) {
  // currently experimental only test if NAPI_VERSION
  // is set to experimental. We can't use C max int
  // as that is not supported as a number on earlier
  // Node.js versions. Once bigint is in a release
  // this should be guarded on the napi version
  // in which bigint was added.
  testModules.splice(testModules.indexOf('bigint'), 1);
  testModules.splice(testModules.indexOf('date'), 1);
  testModules.splice(testModules.indexOf('typedarray-bigint'), 1);
}

if ((process.env.npm_config_NAPI_VERSION !== undefined) &&
    (process.env.npm_config_NAPI_VERSION < 3)) {
  testModules.splice(testModules.indexOf('callbackscope'), 1);
  testModules.splice(testModules.indexOf('version_management'), 1);
}

if ((process.env.npm_config_NAPI_VERSION !== undefined) &&
    (process.env.npm_config_NAPI_VERSION < 4)) {
  testModules.splice(testModules.indexOf('threadsafe_function/threadsafe_function_ptr'), 1);
  testModules.splice(testModules.indexOf('threadsafe_function/threadsafe_function'), 1);
}

if (typeof global.gc === 'function') {
  console.log('Starting test suite\n');

  // Requiring each module runs tests in the module.
  testModules.forEach(name => {
    console.log(`Running test '${name}'`);
    require('./' + name);
  });

  console.log('\nAll tests passed!');
} else {
  // Make it easier to run with the correct (version-dependent) command-line args.
  const child = require('./napi_child').spawnSync(process.argv[0], [ '--expose-gc', __filename ], {
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
