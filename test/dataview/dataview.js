'use strict';

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));

function test(binding) {
  function testDataViewCreation(factory, arrayBuffer, offset, length) {
    const view = factory(arrayBuffer, offset, length);
    offset = offset ? offset : 0;
    assert.ok(dataview.getArrayBuffer(view) instanceof ArrayBuffer);
    assert.strictEqual(dataview.getArrayBuffer(view), arrayBuffer);
    assert.strictEqual(dataview.getByteOffset(view), offset);
    assert.strictEqual(dataview.getByteLength(view),
        length ? length : arrayBuffer.byteLength - offset);
  }

  function testInvalidRange(factory, arrayBuffer, offset, length) {
    assert.throws(() => {
      factory(arrayBuffer, offset, length);
    }, RangeError);
  }

  const dataview = binding.dataview;
  const arrayBuffer = new ArrayBuffer(10);

  testDataViewCreation(dataview.createDataView1, arrayBuffer);
  testDataViewCreation(dataview.createDataView2, arrayBuffer, 2);
  testDataViewCreation(dataview.createDataView2, arrayBuffer, 10);
  testDataViewCreation(dataview.createDataView3, arrayBuffer, 2, 4);
  testDataViewCreation(dataview.createDataView3, arrayBuffer, 10, 0);

  testInvalidRange(dataview.createDataView2, arrayBuffer, 11);
  testInvalidRange(dataview.createDataView3, arrayBuffer, 11, 0);
  testInvalidRange(dataview.createDataView3, arrayBuffer, 6, 5);
}
