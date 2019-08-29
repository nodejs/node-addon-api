'use strict';

const buildType = process.config.target_defaults.default_configuration;

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));

function test(binding) {
  binding.threadsafe_function_ptr.test({}, () => {});
}
