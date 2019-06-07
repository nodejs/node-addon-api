'use strict';
const buildType = process.config.target_defaults.default_configuration;
const common = require('./common');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  const resolving = binding.asyncworker.doWorkNoCallback(true, {});
  resolving.then(common.mustCall()).catch(common.mustNotCall());

  const rejecting = binding.asyncworker.doWorkNoCallback(false, {});
  rejecting.then(common.mustNotCall()).catch(common.mustCall());
  return;
}