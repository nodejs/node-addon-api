'use strict';

let testModules = [
  'arraybuffer',
  'asyncworker',
  'buffer',
  'error',
  'external',
  'function',
  'name',
  'object',
  'typedarray',
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
  process.exitCode = child.status;
}
