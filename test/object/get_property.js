'use strict';

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));

function test(binding) {
  function testGetProperty(nativeGetProperty) {
    const obj = { test: 1 };
    assert.strictEqual(nativeGetProperty(obj, 'test'), 1);
  }

  function testShouldThrowErrorIfKeyIsInvalid(nativeGetProperty) {
    assert.throws(() => {
      nativeGetProperty(undefined, 'test');
    }, /Cannot convert undefined or null to object/);
  }

  testGetProperty(binding.object.getPropertyWithNapiValue);
  testGetProperty(binding.object.getPropertyWithNapiWrapperValue);
  testGetProperty(binding.object.getPropertyWithCStyleString);
  testGetProperty(binding.object.getPropertyWithCppStyleString);

  testShouldThrowErrorIfKeyIsInvalid(binding.object.getPropertyWithNapiValue);
  testShouldThrowErrorIfKeyIsInvalid(binding.object.getPropertyWithNapiWrapperValue);
  testShouldThrowErrorIfKeyIsInvalid(binding.object.getPropertyWithCStyleString);
  testShouldThrowErrorIfKeyIsInvalid(binding.object.getPropertyWithCppStyleString);
}
