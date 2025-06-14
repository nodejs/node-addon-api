'use strict';

const assert = require('assert');
const common = require('./common');

module.exports = common.runTest(test);

async function test (binding) {
  assert.strictEqual(binding.promise.isPromise({}), false);

  const resolving = binding.promise.resolvePromise('resolved');
  await assert.strictEqual(binding.promise.isPromise(resolving), true);
  resolving.then(common.mustCall()).catch(common.mustNotCall());

  const rejecting = binding.promise.rejectPromise('error');
  await assert.strictEqual(binding.promise.isPromise(rejecting), true);
  rejecting.then(common.mustNotCall()).catch(common.mustCall());

  assert(binding.promise.promiseReturnsCorrectEnv());

  const onFulfilled = (value) => value * 2;
  const onRejected = (reason) => reason + '!';

  const thenOnFulfilled = binding.promise.thenMethodOnFulfilled(onFulfilled);
  assert.strictEqual(thenOnFulfilled.isPromise, true);
  const onFulfilledValue = await thenOnFulfilled.promise;
  assert.strictEqual(onFulfilledValue, 84);

  const thenResolve = binding.promise.thenMethodOnFulfilledOnRejectedResolve(onFulfilled, onRejected);
  assert.strictEqual(thenResolve.isPromise, true);
  const thenResolveValue = await thenResolve.promise;
  assert.strictEqual(thenResolveValue, 84);

  const thenRejected = binding.promise.thenMethodOnFulfilledOnRejectedReject(onFulfilled, onRejected);
  assert.strictEqual(thenRejected.isPromise, true);
  const rejectedValue = await thenRejected.promise;
  assert.strictEqual(rejectedValue, 'Rejected!');

  const catchMethod = binding.promise.catchMethod(onRejected);
  assert.strictEqual(catchMethod.isPromise, true);
  const catchValue = await catchMethod.promise;
  assert.strictEqual(catchValue, 'Rejected!');
}
