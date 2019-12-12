'use strict';

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));

function test(binding) {
  function testHasProperty(nativeHasProperty) {
    const obj = { one: 1 };

    Object.defineProperty(obj, 'two', { value: 2 });

    assert.strictEqual(nativeHasProperty(obj, 'one'), true);
    assert.strictEqual(nativeHasProperty(obj, 'two'), true);
    assert.strictEqual('toString' in obj, true);
    assert.strictEqual(nativeHasProperty(obj, 'toString'), true);
  }

  function testShouldThrowErrorIfKeyIsInvalid(nativeHasProperty) {
    assert.throws(() => {
      nativeHasProperty(undefined, 'test');
    }, /Cannot convert undefined or null to object/);
  }

  testHasProperty(binding.object.hasPropertyWithNapiValue);
  testHasProperty(binding.object.hasPropertyWithNapiWrapperValue);
  testHasProperty(binding.object.hasPropertyWithCStyleString);
  testHasProperty(binding.object.hasPropertyWithCppStyleString);

  testShouldThrowErrorIfKeyIsInvalid(binding.object.hasPropertyWithNapiValue);
  testShouldThrowErrorIfKeyIsInvalid(binding.object.hasPropertyWithNapiWrapperValue);
  testShouldThrowErrorIfKeyIsInvalid(binding.object.hasPropertyWithCStyleString);
  testShouldThrowErrorIfKeyIsInvalid(binding.object.hasPropertyWithCppStyleString);
}
