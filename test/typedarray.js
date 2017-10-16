'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  const testData = [
    [ 'int8', Int8Array ],
    [ 'uint8', Uint8Array ],
    [ 'uint8_clamped', Uint8ClampedArray ],
    [ 'int16', Int16Array ],
    [ 'uint16', Uint16Array ],
    [ 'int32', Int32Array ],
    [ 'uint32', Uint32Array ],
    [ 'float32', Float32Array ],
    [ 'float64', Float64Array ],
  ];

  testData.forEach(data => {
    try {
      const length = 4;
      const t = binding.typedarray.createTypedArray(data[0], length);
      assert.ok(t instanceof data[1]);
      assert.strictEqual(binding.typedarray.getTypedArrayType(t), data[0]);
      assert.strictEqual(binding.typedarray.getTypedArrayLength(t), length);

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
