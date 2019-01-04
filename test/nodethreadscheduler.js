'use strict';
const buildType = process.config.target_defaults.default_configuration;
const {mustCall} = require('./common');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  const {nodethreadscheduler} = binding;

  testCalls(50);

  function testCalls(threadCount) {
    nodethreadscheduler.multithreadedCallback(threadCount, mustCall(threadCount));
  }

};
