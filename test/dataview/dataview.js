'use strict';

const assert = require('assert');
module.exports = require('../common').runTest(test);

let runSharedArrayBufferTests = true;

function test (binding) {
  function testDataViewCreation (factory, arrayBuffer, offset, length) {
    const view = factory(arrayBuffer, offset, length);
    offset = offset || 0;
    if (arrayBuffer instanceof ArrayBuffer) {
      assert.ok(dataview.getArrayBuffer(view) instanceof ArrayBuffer);
      assert.strictEqual(dataview.getArrayBuffer(view), arrayBuffer);
    } else {
      assert.ok(dataview.getBuffer(view) instanceof SharedArrayBuffer);
      assert.strictEqual(dataview.getBuffer(view), arrayBuffer);
    }
    assert.strictEqual(dataview.getByteOffset(view), offset);
    assert.strictEqual(dataview.getByteLength(view),
      length || arrayBuffer.byteLength - offset);
  }

  function testInvalidRange (factory, arrayBuffer, offset, length) {
    assert.throws(() => {
      factory(arrayBuffer, offset, length);
    }, RangeError);
  }

  const { hasSharedArrayBuffer, dataview } = binding;

  {
    const arrayBuffer = new ArrayBuffer(10);

    testDataViewCreation(dataview.createDataView, arrayBuffer);
    testDataViewCreation(dataview.createDataViewWithByteOffset, arrayBuffer, 2);
    testDataViewCreation(dataview.createDataViewWithByteOffset, arrayBuffer, 10);
    testDataViewCreation(dataview.createDataViewWithByteOffsetAndByteLength, arrayBuffer, 2, 4);
    testDataViewCreation(dataview.createDataViewWithByteOffsetAndByteLength, arrayBuffer, 10, 0);

    testInvalidRange(dataview.createDataViewWithByteOffset, arrayBuffer, 11);
    testInvalidRange(dataview.createDataViewWithByteOffsetAndByteLength, arrayBuffer, 11, 0);
    testInvalidRange(dataview.createDataViewWithByteOffsetAndByteLength, arrayBuffer, 6, 5);
  }

  if (hasSharedArrayBuffer && runSharedArrayBufferTests) {
    const sab = new SharedArrayBuffer(10);

    try {
      testDataViewCreation(dataview.createDataViewOnSharedArrayBuffer, sab);
    } catch (ex) {
      // The `napi_create_dataview` API does not have a valid `#define`
      // preprocessor guard for SharedArrayBuffer support, so it is
      // possible that the API is present but creating a DataView on
      // SharedArrayBuffer is not supported in the current version of Node.js.
      // In that case, we should skip the test instead of throwing.
      if (ex.message === 'Invalid argument') {
        console.warn(`The current version of Node.js (${process.version}) does not support creating DataViews on SharedArrayBuffers; skipping tests.`);
        runSharedArrayBufferTests = false;
        return;
      }

      throw ex;
    }
    testDataViewCreation(dataview.createDataViewOnSharedArrayBufferWithByteOffset, sab, 2);
    testDataViewCreation(dataview.createDataViewOnSharedArrayBufferWithByteOffset, sab, 10);
    testDataViewCreation(dataview.createDataViewOnSharedArrayBufferWithByteOffsetAndByteLength, sab, 2, 4);
    testDataViewCreation(dataview.createDataViewOnSharedArrayBufferWithByteOffsetAndByteLength, sab, 10, 0);

    testInvalidRange(dataview.createDataViewOnSharedArrayBufferWithByteOffset, sab, 11);
    testInvalidRange(dataview.createDataViewOnSharedArrayBufferWithByteOffsetAndByteLength, sab, 11, 0);
    testInvalidRange(dataview.createDataViewOnSharedArrayBufferWithByteOffsetAndByteLength, sab, 6, 5);
  }
}
