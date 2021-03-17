'use strict';

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));

function test(binding) {
  function testProperty(obj, key, value, nativeGetProperty, nativeSetProperty) {
    nativeSetProperty(obj, key, value);
    assert.strictEqual(nativeGetProperty(obj, key), value);
  }

  testProperty({}, 'key', 'value', binding.object.subscriptGetWithCStyleString, binding.object.subscriptSetWithCStyleString);
  testProperty({ key: 'override me' }, 'key', 'value', binding.object.subscriptGetWithCppStyleString, binding.object.subscriptSetWithCppStyleString);
  testProperty({}, 0, 'value', binding.object.subscriptGetAtIndex, binding.object.subscriptSetAtIndex);
  testProperty({ key: 'override me' }, 0, 'value', binding.object.subscriptGetAtIndex, binding.object.subscriptSetAtIndex);
}
