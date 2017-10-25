'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  var Test = binding.objectwrap.Test;

  function testSetGetMethod(obj) {
    obj.test_set_method(90);
    assert.strictEqual(obj.test_get_method(), 90);
  }

  function testIter(obj) {
    for (const value of obj) {
    }
  }

  function testGetterOnly(obj) {
    obj.test_set_method(91);
    assert.strictEqual(obj.test_getter_only, 91);

    let error;
    try {
      // Can not assign to read only property.
      obj.test_getter_only = 92;
    } catch(e) {
      error = e;
    } finally {
      assert.strictEqual(error.name, 'TypeError');
    }
  }

  function testSetterOnly(obj) {
    obj.test_setter_only = 93;
    assert.strictEqual(obj.test_setter_only, undefined);
    assert.strictEqual(obj.test_getter_only, 93);
  }

  function testGetterSetter(obj) {
    obj.test_getter_setter = 94;
    assert.strictEqual(obj.test_getter_setter, 94);

    obj.test_getter_setter = 95;
    assert.strictEqual(obj.test_getter_setter, 95);
  }

  function testObj(obj) {
    testSetGetMethod(obj);
    testIter(obj);
    testGetterOnly(obj);
    testSetterOnly(obj);
    testGetterSetter(obj);
  }

  testObj(new Test());
  testObj(new Test(1));
  testObj(new Test(1, 2, 3, 4, 5, 6));
  testObj(new Test(1, 2, 3, 4, 5, 6, 7));
}
