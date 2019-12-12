'use strict';

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));

function test(binding) {
  function testDeleteProperty(nativeDeleteProperty) {
    const obj = { one: 1, two: 2 };
    Object.defineProperty(obj, "three", {configurable: false, value: 3});
    assert.strictEqual(nativeDeleteProperty(obj, 'one'), true);
    assert.strictEqual(nativeDeleteProperty(obj, 'missing'), true);

    /* Returns true for all cases except when the property is an own non-
       configurable property, in which case, false is returned in non-strict mode. */
    assert.strictEqual(nativeDeleteProperty(obj, 'three'), false);
    assert.deepStrictEqual(obj, { two: 2 });
  }

  function testShouldThrowErrorIfKeyIsInvalid(nativeDeleteProperty) {
    assert.throws(() => {
      nativeDeleteProperty(undefined, 'test');
    }, /Cannot convert undefined or null to object/);
  }

  testDeleteProperty(binding.object.deletePropertyWithNapiValue);
  testDeleteProperty(binding.object.deletePropertyWithNapiWrapperValue);
  testDeleteProperty(binding.object.deletePropertyWithCStyleString);
  testDeleteProperty(binding.object.deletePropertyWithCppStyleString);

  testShouldThrowErrorIfKeyIsInvalid(binding.object.deletePropertyWithNapiValue);
  testShouldThrowErrorIfKeyIsInvalid(binding.object.deletePropertyWithNapiWrapperValue);
  testShouldThrowErrorIfKeyIsInvalid(binding.object.deletePropertyWithCStyleString);
  testShouldThrowErrorIfKeyIsInvalid(binding.object.deletePropertyWithCppStyleString);
}
