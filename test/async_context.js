'use strict';

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

module.exports = common.runTest(test);

function installAsyncHooksForTest() {
  return new Promise((resolve, reject) => {
    let id;
    const events = [];
    /**
     * TODO(legendecas): investigate why resolving & disabling hooks in
     * destroy callback causing crash with case 'callbackscope.js'.
     */
    let hook;
    let destroyed = false;
    const interval = setInterval(() => {
      if (destroyed) {
        hook.disable();
        clearInterval(interval);
        resolve(events);
      }
    }, 10);

    hook = async_hooks.createHook({
      init(asyncId, type, triggerAsyncId, resource) {
        if (id === undefined && type === 'async_context_test') {
          id = asyncId;
          events.push({ eventName: 'init', type, triggerAsyncId, resource });
        }
      },
      before(asyncId) {
        if (asyncId === id) {
          events.push({ eventName: 'before' });
        }
      },
      after(asyncId) {
        if (asyncId === id) {
          events.push({ eventName: 'after' });
        }
      },
      destroy(asyncId) {
        if (asyncId === id) {
          events.push({ eventName: 'destroy' });
          destroyed = true;
        }
      }
    }).enable();
  });
}

function test(binding) {
  if (!checkAsyncHooks()) {
    return;
  }

  const hooks = installAsyncHooksForTest();
  const triggerAsyncId = async_hooks.executionAsyncId();
  binding.asynccontext.makeCallback(common.mustCall(), { foo: 'foo' });
  return hooks.then(actual => {
      assert.deepStrictEqual(actual, [
        { eventName: 'init',
          type: 'async_context_test',
          triggerAsyncId: triggerAsyncId,
          resource: { foo: 'foo' } },
        { eventName: 'before' },
        { eventName: 'after' },
        { eventName: 'destroy' }
      ]);
  }).catch(common.mustNotCall());
}
