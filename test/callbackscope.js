'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const common = require('./common');

// we only check async hooks on 8.x an higher were
// they are closer to working properly
const nodeVersion = process.versions.node.split('.')[0]
let async_hooks = undefined;
function checkAsyncHooks() {
  if (nodeVersion >= 8) {
    if (async_hooks == undefined) {
      async_hooks = require('async_hooks');
    }
    return true;
  }
  return false;
}

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  if (!checkAsyncHooks())
    return;

  let id;
  let insideHook = false;
  async_hooks.createHook({
    init(asyncId, type, triggerAsyncId, resource) {
      if (id === undefined && type === 'callback_scope_test') {
        id = asyncId;
      }
    },
    before(asyncId) {
      if (asyncId === id)
        insideHook = true;
    },
    after(asyncId) {
      if (asyncId === id)
        insideHook = false;
    }
  }).enable();

  binding.callbackscope.runInCallbackScope(function() {
    assert(insideHook);
  });
}
