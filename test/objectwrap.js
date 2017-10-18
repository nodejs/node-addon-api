'use strict';
const assert = require('assert');

test(require('./load-bindings')('binding'));
test(require('./load-bindings')('binding_noexcept'));

function test(binding) {
  var Test = binding.objectwrap.Test;

  function testSetGet(obj) {
    obj.test_set(90);
    assert.strictEqual(obj.test_get(), 90);
  }

  function testObj(obj) {
    testSetGet(obj);
  }

  testObj(new Test());
  testObj(new Test(1));
  testObj(new Test(1, 2, 3, 4, 5, 6));
  testObj(new Test(1, 2, 3, 4, 5, 6, 7));
}
