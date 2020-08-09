'use strict';
const buildType = process.config.target_defaults.default_configuration;
const common = require('./common');

module.exports = test(require(`./build/${buildType}/binding.node`))
  .then(() => test(require(`./build/${buildType}/binding_noexcept.node`)));

async function test(binding) {
  await binding.asyncworker.doWorkNoCallback(true, {})
    .then(common.mustCall()).catch(common.mustNotCall());

  await binding.asyncworker.doWorkNoCallback(false, {})
    .then(common.mustNotCall()).catch(common.mustCall());
}
