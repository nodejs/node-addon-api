'use strict';
const assert = require('assert');
const common = require('./common');

test(require('./load-bindings')('binding'));
test(require('./load-bindings')('binding_noexcept'));

function test(binding) {
  assert.strictEqual(binding.promise.isPromise({}), false);

  const resolving = binding.promise.resolvePromise('resolved');
  assert.strictEqual(binding.promise.isPromise(resolving), true);
  resolving.then(common.mustCall()).catch(common.mustNotCall());

  const rejecting = binding.promise.rejectPromise('error');
  assert.strictEqual(binding.promise.isPromise(rejecting), true);
  rejecting.then(common.mustNotCall()).catch(common.mustCall());
}
