'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));

function test(binding) {
  function assertPropertyIs(obj, key, attribute) {
    const propDesc = Object.getOwnPropertyDescriptor(obj, key);
    assert.ok(propDesc);
    assert.ok(propDesc[attribute]);
  }

  function assertPropertyIsNot(obj, key, attribute) {
    const propDesc = Object.getOwnPropertyDescriptor(obj, key);
    assert.ok(propDesc);
    assert.ok(!propDesc[attribute]);
  }

  function testDefineProperties(nameType) {
    const obj = {};
    binding.object.defineProperties(obj, nameType);

    // accessors
    assertPropertyIsNot(obj, 'readonlyAccessor', 'enumerable');
    assertPropertyIsNot(obj, 'readonlyAccessor', 'configurable');
    assert.strictEqual(obj.readonlyAccessor, true);

    assertPropertyIsNot(obj, 'readonlyAccessorWithUserData', 'enumerable');
    assertPropertyIsNot(obj, 'readonlyAccessorWithUserData', 'configurable');
    assert.strictEqual(obj.readonlyAccessorWithUserData, 1234, nameType);

    assertPropertyIsNot(obj, 'readwriteAccessor', 'enumerable');
    assertPropertyIsNot(obj, 'readwriteAccessor', 'configurable');
    obj.readwriteAccessor = false;
    assert.strictEqual(obj.readwriteAccessor, false);
    obj.readwriteAccessor = true;
    assert.strictEqual(obj.readwriteAccessor, true);

    assertPropertyIsNot(obj, 'readwriteAccessorWithUserData', 'enumerable');
    assertPropertyIsNot(obj, 'readwriteAccessorWithUserData', 'configurable');
    obj.readwriteAccessorWithUserData = 2;
    assert.strictEqual(obj.readwriteAccessorWithUserData, 2);
    obj.readwriteAccessorWithUserData = -14;
    assert.strictEqual(obj.readwriteAccessorWithUserData, -14);

    // templated accessors
    assertPropertyIsNot(obj, 'readonlyAccessorT', 'enumerable');
    assertPropertyIsNot(obj, 'readonlyAccessorT', 'configurable');
    assert.strictEqual(obj.readonlyAccessorT, true);

    assertPropertyIsNot(obj, 'readonlyAccessorWithUserDataT', 'enumerable');
    assertPropertyIsNot(obj, 'readonlyAccessorWithUserDataT', 'configurable');
    assert.strictEqual(obj.readonlyAccessorWithUserDataT, -14, nameType);

    assertPropertyIsNot(obj, 'readwriteAccessorT', 'enumerable');
    assertPropertyIsNot(obj, 'readwriteAccessorT', 'configurable');
    obj.readwriteAccessorT = false;
    assert.strictEqual(obj.readwriteAccessorT, false);
    obj.readwriteAccessorT = true;
    assert.strictEqual(obj.readwriteAccessorT, true);

    assertPropertyIsNot(obj, 'readwriteAccessorWithUserDataT', 'enumerable');
    assertPropertyIsNot(obj, 'readwriteAccessorWithUserDataT', 'configurable');
    obj.readwriteAccessorWithUserDataT = 2;
    assert.strictEqual(obj.readwriteAccessorWithUserDataT, 2);
    obj.readwriteAccessorWithUserDataT = -14;
    assert.strictEqual(obj.readwriteAccessorWithUserDataT, -14);

    // values
    assertPropertyIsNot(obj, 'readonlyValue', 'writable');
    assertPropertyIsNot(obj, 'readonlyValue', 'enumerable');
    assertPropertyIsNot(obj, 'readonlyValue', 'configurable');
    assert.strictEqual(obj.readonlyValue, true);

    assertPropertyIs(obj, 'readwriteValue', 'writable');
    assertPropertyIsNot(obj, 'readwriteValue', 'enumerable');
    assertPropertyIsNot(obj, 'readwriteValue', 'configurable');
    obj.readwriteValue = false;
    assert.strictEqual(obj.readwriteValue, false);
    obj.readwriteValue = true;
    assert.strictEqual(obj.readwriteValue, true);

    assertPropertyIsNot(obj, 'enumerableValue', 'writable');
    assertPropertyIs(obj, 'enumerableValue', 'enumerable');
    assertPropertyIsNot(obj, 'enumerableValue', 'configurable');

    assertPropertyIsNot(obj, 'configurableValue', 'writable');
    assertPropertyIsNot(obj, 'configurableValue', 'enumerable');
    assertPropertyIs(obj, 'configurableValue', 'configurable');

    // functions
    assertPropertyIsNot(obj, 'function', 'writable');
    assertPropertyIsNot(obj, 'function', 'enumerable');
    assertPropertyIsNot(obj, 'function', 'configurable');
    assert.strictEqual(obj.function(), true);
    assert.strictEqual(obj.functionWithUserData(), obj.readonlyAccessorWithUserDataT);
  }

  testDefineProperties('literal');
  testDefineProperties('string');
  testDefineProperties('value');

  {
    const obj = {};
    const testSym = Symbol();
    binding.object.defineValueProperty(obj, testSym, 1);
    assert.strictEqual(obj[testSym], 1);
  }

  {
    const obj = {'one': 1, 'two': 2, 'three': 3};
    var arr = binding.object.GetPropertyNames(obj);
    assert.deepStrictEqual(arr, ['one', 'two', 'three'])
  }

  {
    const magicObject = binding.object.createObjectUsingMagic();
    assert.deepStrictEqual(magicObject, {
      0: 0,
      42: 120,
      cp_false: false,
      cp_true: true,
      s_true: true,
      s_false: false,
      '0.0f': 0,
      '0.0': 0,
      '-1': -1,
      foo2: 'foo',
      foo4: 'foo',
      foo5: 'foo',
      foo6: 'foo',
      foo7: 'foo',
      circular: magicObject,
      circular2: magicObject
    });
  }
}
