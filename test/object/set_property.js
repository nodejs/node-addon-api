'use strict';

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));

function test(binding) {
  function testSetProperty(nativeSetProperty) {
    const obj = {};
    nativeSetProperty(obj, 'test', 1);
    assert.strictEqual(obj.test, 1);
  }

  function testShouldThrowErrorIfKeyIsInvalid(nativeSetProperty) {
    assert.throws(() => {
      nativeSetProperty(undefined, 'test', 1);
    }, /Cannot convert undefined or null to object/);
  }

  testSetProperty(binding.object.setPropertyWithNapiValue);
  testSetProperty(binding.object.setPropertyWithNapiWrapperValue);
  testSetProperty(binding.object.setPropertyWithCStyleString);
  testSetProperty(binding.object.setPropertyWithCppStyleString);

  testShouldThrowErrorIfKeyIsInvalid(binding.object.setPropertyWithNapiValue);
  testShouldThrowErrorIfKeyIsInvalid(binding.object.setPropertyWithNapiWrapperValue);
  testShouldThrowErrorIfKeyIsInvalid(binding.object.setPropertyWithCStyleString);
  testShouldThrowErrorIfKeyIsInvalid(binding.object.setPropertyWithCppStyleString);
}
