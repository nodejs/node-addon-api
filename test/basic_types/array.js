'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));

function test(binding) {

  // create empty array
  const array = binding.basic_types_array.createArray();
  assert.strictEqual(binding.basic_types_array.getLength(array), 0);

  // create array with length
  const arrayWithLength = binding.basic_types_array.createArray(10);
  assert.strictEqual(binding.basic_types_array.getLength(arrayWithLength), 10);

  // set function test
  binding.basic_types_array.set(array, 0, 10);
  binding.basic_types_array.set(array, 1, "test");
  binding.basic_types_array.set(array, 2, 3.0);

  // check length after set data
  assert.strictEqual(binding.basic_types_array.getLength(array), 3);

  // get function test
  assert.strictEqual(binding.basic_types_array.get(array, 0), 10);
  assert.strictEqual(binding.basic_types_array.get(array, 1), "test");
  assert.strictEqual(binding.basic_types_array.get(array, 2), 3.0);

  // overwrite test
  binding.basic_types_array.set(array, 0, 5);
  assert.strictEqual(binding.basic_types_array.get(array, 0), 5);

  // out of index test
  assert.strictEqual(binding.basic_types_array.get(array, 5), undefined);
}
