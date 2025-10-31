'use strict';

const assert = require('assert');

module.exports = require('./common').runTest(test);

let skippedMessageShown = false;

function test ({ hasSharedArrayBuffer, sharedarraybuffer }) {
  if (!hasSharedArrayBuffer) {
    if (!skippedMessageShown) {
      console.log('   >Skipped (no SharedArrayBuffer support)');
      skippedMessageShown = true;
    }
    return;
  }

  {
    const sab = new SharedArrayBuffer(16);
    const ab = new ArrayBuffer(16);
    const obj = {};
    const arr = [];

    assert.strictEqual(sharedarraybuffer.testIsSharedArrayBuffer(sab), true);
    assert.strictEqual(sharedarraybuffer.testIsSharedArrayBuffer(ab), false);
    assert.strictEqual(sharedarraybuffer.testIsSharedArrayBuffer(obj), false);
    assert.strictEqual(sharedarraybuffer.testIsSharedArrayBuffer(arr), false);
    assert.strictEqual(sharedarraybuffer.testIsSharedArrayBuffer(null), false);
    assert.strictEqual(sharedarraybuffer.testIsSharedArrayBuffer(undefined), false);
  }

  {
    const sab = sharedarraybuffer.testCreateSharedArrayBuffer(16);
    assert(sab instanceof SharedArrayBuffer);
    assert.strictEqual(sab.byteLength, 16);
  }

  {
    const sab = new SharedArrayBuffer(32);
    const byteLength = sharedarraybuffer.testGetSharedArrayBufferInfo(sab);
    assert.strictEqual(byteLength, 32);
  }

  {
    const sab = new SharedArrayBuffer(8);
    const result = sharedarraybuffer.testSharedArrayBufferData(sab);
    assert.strictEqual(result, true);

    // Check if data was written correctly
    const view = new Uint8Array(sab);
    for (let i = 0; i < 8; i++) {
      assert.strictEqual(view[i], i % 256);
    }
  }
}
