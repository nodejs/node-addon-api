'use strict';

const assert = require('assert');

module.exports = require('./common').runTest(test);

function test (binding) {
  const testData = [
    ['int8', Int8Array, 1],
    ['uint8', Uint8Array, 1],
    ['uint8_clamped', Uint8ClampedArray, 1],
    ['int16', Int16Array, 2],
    ['uint16', Uint16Array, 2],
    ['int32', Int32Array, 4],
    ['uint32', Uint32Array, 4],
    ['float32', Float32Array, 4],
    ['float64', Float64Array, 8]
  ];

  testData.forEach(data => {
    try {
      const length = 4;
      const t = binding.typedarray.createTypedArray(data[0], length);
      assert.ok(t instanceof data[1]);
      assert.strictEqual(binding.typedarray.getTypedArrayType(t), data[0]);
      assert.strictEqual(binding.typedarray.getTypedArrayLength(t), length);
      assert.strictEqual(binding.typedarray.getTypedArraySize(t), data[2]);

      t[3] = 11;
      assert.strictEqual(binding.typedarray.getTypedArrayElement(t, 3), 11);
      binding.typedarray.setTypedArrayElement(t, 3, 22);
      assert.strictEqual(binding.typedarray.getTypedArrayElement(t, 3), 22);
      assert.strictEqual(t[3], 22);

      const b = binding.typedarray.getTypedArrayBuffer(t);
      assert.ok(b instanceof ArrayBuffer);
    } catch (e) {
      console.log(data);
      throw e;
    }
  });

  testData.forEach(data => {
    try {
      const length = 4;
      const offset = 8;
      const b = new ArrayBuffer(offset + 64 * 4);

      const t = binding.typedarray.createTypedArray(data[0], length, b, offset);
      assert.ok(t instanceof data[1]);
      assert.strictEqual(binding.typedarray.getTypedArrayType(t), data[0]);
      assert.strictEqual(binding.typedarray.getTypedArrayLength(t), length);
      assert.strictEqual(binding.typedarray.getTypedArraySize(t), data[2]);

      t[3] = 11;
      assert.strictEqual(binding.typedarray.getTypedArrayElement(t, 3), 11);
      binding.typedarray.setTypedArrayElement(t, 3, 22);
      assert.strictEqual(binding.typedarray.getTypedArrayElement(t, 3), 22);
      assert.strictEqual(t[3], 22);

      assert.strictEqual(binding.typedarray.getTypedArrayBuffer(t), b);
    } catch (e) {
      console.log(data);
      throw e;
    }
  });

  assert.throws(() => {
    binding.typedarray.createInvalidTypedArray();
  }, /Invalid (pointer passed as )?argument/);
}
