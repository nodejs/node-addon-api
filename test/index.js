'use strict';

process.config.target_defaults.default_configuration =
  require('fs')
    .readdirSync(require('path').join(__dirname, 'build'))
    .filter((item) => (item === 'Debug' || item === 'Release'))[0];

let testModules = [
  'arraybuffer',
  'asyncworker',
  'buffer',
  'error',
  'external',
  'function',
  'name',
  'object',
  'promise',
  'typedarray',
  'objectwrap'
];

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
