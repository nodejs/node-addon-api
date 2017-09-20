'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

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
