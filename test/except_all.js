'use strict';

const assert = require('assert');

module.exports = require('./common').runTestWithBuildType(test);

function test (buildType) {
  const binding = require(`./build/${buildType}/binding_except_all.node`);

  const message = 'error message';
  assert.throws(binding.throwStdException.bind(undefined, message), { message });

  assert.throws(binding.throwPrimitiveException.bind(undefined), { message: 'A native exception was thrown' });
}
