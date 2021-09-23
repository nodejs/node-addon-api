'use strict';
const assert = require('assert');

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

module.exports = require('./common').runTest(test);

function test(binding) {
  if (!checkAsyncHooks())
    return;

  let id;
  let insideHook = false;
  const hook = async_hooks.createHook({
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

  return new Promise(resolve => {
    binding.callbackscope.runInCallbackScope(function() {
      assert(insideHook);
      hook.disable();
      resolve();
    });
  });
}
