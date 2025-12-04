'use strict';

const assert = require('assert');

const test = bindingName => {
  const binding = require(bindingName);
  const TestOverride = binding.objectwrap_constructor_trait_tag.TestOverride;
  // TODO(legendecas): Expose Object.setPrototypeOf in node-api.
  Object.setPrototypeOf(TestOverride, EventTarget);
  Object.setPrototypeOf(TestOverride.prototype, EventTarget.prototype);

  const test = new TestOverride();
  Object.setPrototypeOf(test, TestOverride.prototype);

  // Verify that the TestOverride class is an EventTarget instance and has both
  // EventTarget brand and TestOverride brand.
  assert(test instanceof EventTarget);
  assert(test instanceof TestOverride);
  // Verify TestOverride brand.
  assert.strictEqual(test.isTestOverride, true);
  // Verify EventTarget brand.
  test.addEventListener('test', () => {});
};

module.exports = require('./common').runTestWithBindingPath(test);
