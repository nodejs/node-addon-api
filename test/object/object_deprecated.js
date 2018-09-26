'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));

function test(binding) {
  if (!('object_deprecated' in binding)) {
    return;
  }
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

    assertPropertyIsNot(obj, 'readonlyAccessor', 'enumerable');
    assertPropertyIsNot(obj, 'readonlyAccessor', 'configurable');
    assert.strictEqual(obj.readonlyAccessor, true);

    assertPropertyIsNot(obj, 'readwriteAccessor', 'enumerable');
    assertPropertyIsNot(obj, 'readwriteAccessor', 'configurable');
    obj.readwriteAccessor = false;
    assert.strictEqual(obj.readwriteAccessor, false);
    obj.readwriteAccessor = true;
    assert.strictEqual(obj.readwriteAccessor, true);

    assertPropertyIsNot(obj, 'function', 'writable');
    assertPropertyIsNot(obj, 'function', 'enumerable');
    assertPropertyIsNot(obj, 'function', 'configurable');
    assert.strictEqual(obj.function(), true);
  }

  testDefineProperties('literal');
  testDefineProperties('string');
  testDefineProperties('value');
}
