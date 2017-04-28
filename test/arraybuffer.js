'use strict';
const buildType = process.config.target_defaults.default_configuration;
const binding = require(`./build/${buildType}/binding.node`);
const assert = require('assert');

let test = binding.arraybuffer.createBuffer();
binding.arraybuffer.checkBuffer(test);
assert.ok(test instanceof ArrayBuffer);

let test2 = test.slice(0);
binding.arraybuffer.checkBuffer(test2);

test = binding.arraybuffer.createExternalBuffer();
binding.arraybuffer.checkBuffer(test);
assert.ok(test instanceof ArrayBuffer);

test = binding.arraybuffer.createExternalBufferWithFinalize();
binding.arraybuffer.checkBuffer(test);
assert.ok(test instanceof ArrayBuffer);
assert.strictEqual(0, binding.arraybuffer.getFinalizeCount());
test = null;
global.gc();
assert.strictEqual(1, binding.arraybuffer.getFinalizeCount());

test = binding.arraybuffer.createExternalBufferWithFinalizeHint();
binding.arraybuffer.checkBuffer(test);
assert.ok(test instanceof ArrayBuffer);
assert.strictEqual(1, binding.arraybuffer.getFinalizeCount());
test = null;
global.gc();
assert.strictEqual(2, binding.arraybuffer.getFinalizeCount());
