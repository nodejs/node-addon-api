'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const common = require('./common');

// we only check async hooks on 8.x and higher where
// they are closer to working properly
const nodeVersion = process.versions.node.split('.')[0]
let async_hooks = undefined;
function checkAsyncHooks() {
  if (nodeVersion >=8) {
    if (async_hooks == undefined) {
      async_hooks = require('async_hooks');
    }
    return true;
  }
  return false;
}

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function installAsyncHooksForTest() {
  return new Promise((resolve, reject) => {
    let id;
    const events = [];
    const hook = async_hooks.createHook({
      init(asyncId, type, triggerAsyncId, resource) {
        if (id === undefined && type === 'TestResource') {
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
          hook.disable();
          resolve(events);
        }
      }
    }).enable();
  });
}

function test(binding) {
  if (!checkAsyncHooks()) {
    binding.asyncpromise.makePromise(1, {}, 'early success')
      .then(function (r) {
        assert.strictEqual(r, 'early success', 'AsyncPromise test [succeed before queue] failed to resolve with expected results');
      })
      .catch(function (e) {
        assert.fail('AsyncPromise test [succeed before queue] failed with object "' + e + '"');
      });

    binding.asyncpromise.makePromise(2, {}, 'late success')
      .then(function (r) {
        assert.strictEqual(r, 'late success', 'AsyncPromise test [succeed in execute] failed to resolve with expected results');
      })
      .catch(function (e) {
        assert.fail('AsyncPromise test [succeed in execute] failed with object "' + e + '"');
      });

    binding.asyncpromise.makePromise(3, {}, 'early failure')
      .then(function (r) {
        assert.fail('AsyncPromise test [fail before queue] was resolved in error');
      },
      function (e) {
        assert.strictEqual(e.message, 'early failure', 'AsyncPromise test [fail before queue] failed with unexpected message "' + e.message + '"');
      })
      .catch(function (e) {
        assert.fail('AsyncPromise test [fail before queue] failed with thrown object "' + e + '"');
      });

    binding.asyncpromise.makePromise(4, {}, 'late failure')
      .then(function (r) {
        assert.fail('AsyncPromise test [fail in execute] was resolved in error');
      },
      function (e) {
        assert.strictEqual(e.message, 'late failure', 'AsyncPromise test [fail in execute] failed with unexpected message "' + e.message + '"');
      })
      .catch(function (e) {
        assert.fail('AsyncPromise test [fail in execute] failed with thrown object "' + e + '"');
      });

    return;
  }

  {
    const hooks = installAsyncHooksForTest();
    const triggerAsyncId = async_hooks.executionAsyncId();
    binding.asyncpromise.makePromise(1, { foo: 'foo' }, 'early success')
      .then(function (e) {
        assert.strictEqual(r, 'early success', 'AsyncPromise test [succeed before queue] failed to resolve with expected results');
      })
      .catch(function (e) {
        assert.fail('AsyncPromise test [succeed before queue] failed with object "' + e + '"');
      });

    hooks.then(actual => {
      assert.deepStrictEqual(actual, [
        { eventName: 'init',
          type: 'TestResource',
          triggerAsyncId: triggerAsyncId,
          resource: { foo: 'foo' } },
        { eventName: 'before' },
        { eventName: 'after' },
        { eventName: 'destroy' }
      ]);
    }).catch(common.mustNotCall());
  }

  {
    const hooks = installAsyncHooksForTest();
    const triggerAsyncId = async_hooks.executionAsyncId();

    binding.asyncpromise.makePromise(2, { foo: 'foo' }, 'late success')
      .then(function (r) {
        assert.strictEqual(r, 'late success', 'AsyncPromise test [succeed in execute] failed to resolve with expected results');
      })
      .catch(function (e) {
        assert.fail('AsyncPromise test [succeed in execute] failed with object "' + e + '"');
      });

    hooks.then(actual => {
      assert.deepStrictEqual(actual, [
        { eventName: 'init',
          type: 'TestResource',
          triggerAsyncId: triggerAsyncId,
          resource: { foo: 'foo' } },
        { eventName: 'before' },
        { eventName: 'after' },
        { eventName: 'destroy' }
      ]);
    }).catch(common.mustNotCall());
  }

  {
    const hooks = installAsyncHooksForTest();
    const triggerAsyncId = async_hooks.executionAsyncId();

    binding.asyncpromise.makePromise(3, { foo: 'foo' }, 'early failure')
      .then(function (r) {
        assert.fail('AsyncPromise test [fail before queue] was resolved in error');
      },
      function (e) {
        assert.strictEqual(e.message, 'early failure', 'AsyncPromise test [fail before queue] failed with unexpected message "' + e.message + '"');
      })
      .catch(function (e) {
        assert.fail('AsyncPromise test [fail before queue] failed with thrown object "' + e + '"');
      });

    hooks.then(actual => {
      assert.deepStrictEqual(actual, [
        { eventName: 'init',
          type: 'TestResource',
          triggerAsyncId: triggerAsyncId,
          resource: { foo: 'foo' } },
        { eventName: 'before' },
        { eventName: 'after' },
        { eventName: 'destroy' }
      ]);
    }).catch(common.mustNotCall());
  }

  {
    const hooks = installAsyncHooksForTest();
    const triggerAsyncId = async_hooks.executionAsyncId();

    binding.asyncpromise.makePromise(4, { foo: 'foo' }, 'late failure')
      .then(function (r) {
        assert.fail('AsyncPromise test [fail in execute] was resolved in error');
      },
      function (e) {
        assert.strictEqual(e.message, 'late failure', 'AsyncPromise test [fail in execute] failed with unexpected message "' + e.message + '"');
      })
      .catch(function (e) {
        assert.fail('AsyncPromise test [fail in execute] failed with thrown object "' + e + '"');
      });

    hooks.then(actual => {
      assert.deepStrictEqual(actual, [
        { eventName: 'init',
          type: 'TestResource',
          triggerAsyncId: triggerAsyncId,
          resource: { foo: 'foo' } },
        { eventName: 'before' },
        { eventName: 'after' },
        { eventName: 'destroy' }
      ]);
    }).catch(common.mustNotCall());
  }
}
