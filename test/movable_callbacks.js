'use strict';
const buildType = process.config.target_defaults.default_configuration;
const common = require('./common');
const testUtil = require('./testUtil');

Promise.all([
  test(require(`./build/${buildType}/binding.node`).movable_callbacks),
  test(require(`./build/${buildType}/binding_noexcept.node`).movable_callbacks),
]).catch(e => {
  console.error(e);
  process.exitCode = 1;
});

async function test(binding) {
  await testUtil.runGCTests([
    'External',
    () => {
      const fn = common.mustCall(() => {
        // noop
      }, 1);
      const external = binding.createExternal(fn);
    },
    () => {
      // noop, wait for gc
    }
  ]);
}
