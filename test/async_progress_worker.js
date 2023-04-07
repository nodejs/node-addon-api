'use strict';

const common = require('./common');
const assert = require('assert');

module.exports = common.runTest(test);
const nodeVersion = process.versions.node.split('.')[0];

let asyncHooks;
function checkAsyncHooks () {
  if (nodeVersion >= 8) {
    if (asyncHooks === undefined) {
      asyncHooks = require('async_hooks');
    }
    return true;
  }
  return false;
}

function installAsyncHooksForTest (resName) {
  return new Promise((resolve, reject) => {
    let id;
    const events = [];
    /**
     * TODO(legendecas): investigate why resolving & disabling hooks in
     * destroy callback causing crash with case 'callbackscope.js'.
     */
    let destroyed = false;
    const hook = asyncHooks.createHook({
      init (asyncId, type, triggerAsyncId, resource) {
        if (id === undefined && type === resName) {
          id = asyncId;
          events.push({ eventName: 'init', type, triggerAsyncId, resource });
        }
      },
      before (asyncId) {
        if (asyncId === id) {
          events.push({ eventName: 'before' });
        }
      },
      after (asyncId) {
        if (asyncId === id) {
          events.push({ eventName: 'after' });
        }
      },
      destroy (asyncId) {
        if (asyncId === id) {
          events.push({ eventName: 'destroy' });
          destroyed = true;
        }
      }
    }).enable();

    const interval = setInterval(() => {
      if (destroyed) {
        hook.disable();
        clearInterval(interval);
        resolve(events);
      }
    }, 10);
  });
}

async function test ({ asyncprogressworker }) {
  await success(asyncprogressworker);
  await fail(asyncprogressworker);
  await signalTest(asyncprogressworker.doMalignTest);
  await signalTest(asyncprogressworker.doSignalAfterProgressTest);

  await asyncProgressWorkerCallbackOverloads(asyncprogressworker.runWorkerWithCb);
  await asyncProgressWorkerRecvOverloads(asyncprogressworker.runWorkerWithRecv);
  await asyncProgressWorkerNoCbOverloads(asyncprogressworker.runWorkerNoCb);
}

async function asyncProgressWorkerCallbackOverloads (bindingFunction) {
  bindingFunction(common.mustCall());
  if (!checkAsyncHooks()) {
    return;
  }
  const hooks = installAsyncHooksForTest('cbResources');
  const triggerAsyncId = asyncHooks.executionAsyncId();
  await new Promise((resolve, reject) => {
    bindingFunction(common.mustCall(), 'cbResources');
    hooks.then(actual => {
      assert.deepStrictEqual(actual, [
        {
          eventName: 'init',
          type: 'cbResources',
          triggerAsyncId: triggerAsyncId,
          resource: {}
        },
        { eventName: 'before' },
        { eventName: 'after' },
        { eventName: 'destroy' }
      ]);
    }).catch(common.mustNotCall());
    resolve();
  });
}

async function asyncProgressWorkerRecvOverloads (bindingFunction) {
  const recvObject = {
    a: 4
  };

  function cb () {
    assert.strictEqual(this.a, recvObject.a);
  }

  bindingFunction(recvObject, common.mustCall(cb));
  if (!checkAsyncHooks()) {
    return;
  }
  const asyncResources = [
    { resName: 'cbRecvResources', resObject: {} },
    { resName: 'cbRecvResourcesObject', resObject: { foo: 'bar' } }
  ];

  for (const asyncResource of asyncResources) {
    const asyncResName = asyncResource.resName;
    const asyncResObject = asyncResource.resObject;

    const hooks = installAsyncHooksForTest(asyncResource.resName);
    const triggerAsyncId = asyncHooks.executionAsyncId();
    await new Promise((resolve, reject) => {
      if (Object.keys(asyncResObject).length === 0) {
        bindingFunction(recvObject, common.mustCall(cb), asyncResName);
      } else {
        bindingFunction(recvObject, common.mustCall(cb), asyncResName, asyncResObject);
      }

      hooks.then(actual => {
        assert.deepStrictEqual(actual, [
          {
            eventName: 'init',
            type: asyncResName,
            triggerAsyncId: triggerAsyncId,
            resource: asyncResObject
          },
          { eventName: 'before' },
          { eventName: 'after' },
          { eventName: 'destroy' }
        ]);
      }).catch(common.mustNotCall());
      resolve();
    });
  }
}

async function asyncProgressWorkerNoCbOverloads (bindingFunction) {
  bindingFunction(common.mustCall(() => {}));
  if (!checkAsyncHooks()) {
    return;
  }
  const asyncResources = [
    { resName: 'noCbResources', resObject: {} },
    { resName: 'noCbResourcesObject', resObject: { foo: 'bar' } }
  ];

  for (const asyncResource of asyncResources) {
    const asyncResName = asyncResource.resName;
    const asyncResObject = asyncResource.resObject;

    const hooks = installAsyncHooksForTest(asyncResName);
    const triggerAsyncId = asyncHooks.executionAsyncId();
    await new Promise((resolve, reject) => {
      if (Object.keys(asyncResObject).length === 0) {
        bindingFunction(asyncResName, common.mustCall(() => {}));
      } else {
        bindingFunction(asyncResName, asyncResObject, common.mustCall(() => {}));
      }

      hooks.then(actual => {
        assert.deepStrictEqual(actual, [
          {
            eventName: 'init',
            type: asyncResName,
            triggerAsyncId: triggerAsyncId,
            resource: asyncResObject
          },
          { eventName: 'before' },
          { eventName: 'after' },
          { eventName: 'destroy' }
        ]);
      }).catch(common.mustNotCall());
      resolve();
    });
  }
}

function success (binding) {
  return new Promise((resolve, reject) => {
    const expected = [0, 1, 2, 3];
    const actual = [];
    binding.doWork(expected.length,
      common.mustCall((err) => {
        if (err) {
          reject(err);
        }
      }),
      common.mustCall((_progress) => {
        actual.push(_progress);
        if (actual.length === expected.length) {
          assert.deepEqual(actual, expected);
          resolve();
        }
      }, expected.length)
    );
  });
}

function fail (binding) {
  return new Promise((resolve) => {
    binding.doWork(-1,
      common.mustCall((err) => {
        assert.throws(() => { throw err; }, /test error/);
        resolve();
      }),
      common.mustNotCall()
    );
  });
}

function signalTest (bindingFunction) {
  return new Promise((resolve, reject) => {
    bindingFunction(
      common.mustCall((err) => {
        if (err) {
          return reject(err);
        }
        resolve();
      }),
      common.mustCallAtLeast((error, reason) => {
        try {
          assert(!error, reason);
        } catch (e) {
          reject(e);
        }
      }, 1)
    );
  });
}
