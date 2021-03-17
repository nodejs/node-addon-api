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

  function testShouldReturnUndefinedIfKeyIsNotPresent(nativeGetProperty) {
    const obj = { };
    assert.strictEqual(nativeGetProperty(obj, 'test'), undefined);
  }

  function testShouldThrowErrorIfKeyIsInvalid(nativeGetProperty) {
    assert.throws(() => {
      nativeGetProperty(undefined, 'test');
    }, /Cannot convert undefined or null to object/);
  }

  const nativeFunctions = [
    binding.object.getPropertyWithNapiValue,
    binding.object.getPropertyWithNapiWrapperValue,
    binding.object.getPropertyWithCStyleString,
    binding.object.getPropertyWithCppStyleString
  ];

  nativeFunctions.forEach((nativeFunction) => {
    testGetProperty(nativeFunction);
    testShouldReturnUndefinedIfKeyIsNotPresent(nativeFunction);
    testShouldThrowErrorIfKeyIsInvalid(nativeFunction);
  });
}
