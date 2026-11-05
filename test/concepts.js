'use strict';

const assert = require('assert');

module.exports = require('./common').runTest(test);

function test (binding, context) {
  assert.ok(binding.concepts);
  assert.strictEqual(typeof binding.concepts.hasConcepts, 'boolean');

  if (context && context.bindingPath && context.bindingPath.includes('binding_cpp20')) {
    assert.strictEqual(binding.concepts.hasConcepts, true);
  }

  // Verify function created with concept-constrained CallbackCallable
  assert.strictEqual(binding.concepts.testFunction(), 'concept callback ok');

  // Verify property descriptor created with concept-constrained GetterCallable & SetterCallable
  assert.strictEqual(binding.concepts.testAccessor.testProp, 42);
  binding.concepts.testAccessor.testProp = 100;
  assert.strictEqual(binding.concepts.testAccessor.testProp, 42);

  // Verify function descriptor created with concept-constrained CallbackCallable
  assert.strictEqual(binding.concepts.testFunctionDescriptor.testMethod(), true);

  // Verify finalizer-attached object
  assert.ok(binding.concepts.testFinalizer);

  // Verify array buffer with concept-constrained finalizer
  assert.ok(binding.concepts.testArrayBuffer instanceof ArrayBuffer);
  assert.strictEqual(binding.concepts.testArrayBuffer.byteLength, 16);

  // Verify buffer with concept-constrained finalizer
  assert.ok(Buffer.isBuffer(binding.concepts.testBuffer));
  assert.strictEqual(binding.concepts.testBuffer.length, 8);
}
