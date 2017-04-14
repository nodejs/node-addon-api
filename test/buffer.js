'use strict';
const buildType = process.config.target_defaults.default_configuration;
const binding = require(`./build/${buildType}/binding.node`);
const assert = require('assert');

let test = binding.buffer.createBuffer();
binding.buffer.checkBuffer(test);
assert.ok(test instanceof Buffer);

let test2 = Buffer.alloc(test.length);
test.copy(test2);
binding.buffer.checkBuffer(test2);

test = binding.buffer.createBufferCopy();
binding.buffer.checkBuffer(test);
assert.ok(test instanceof Buffer);

test = binding.buffer.createExternalBuffer();
binding.buffer.checkBuffer(test);
assert.ok(test instanceof Buffer);

test = binding.buffer.createExternalBufferWithFinalize();
binding.buffer.checkBuffer(test);
assert.ok(test instanceof Buffer);
assert.strictEqual(0, binding.buffer.getFinalizeCount());
test = null;
global.gc();
assert.strictEqual(1, binding.buffer.getFinalizeCount());

test = binding.buffer.createExternalBufferWithFinalizeHint();
binding.buffer.checkBuffer(test);
assert.ok(test instanceof Buffer);
assert.strictEqual(1, binding.buffer.getFinalizeCount());
test = null;
global.gc();
assert.strictEqual(2, binding.buffer.getFinalizeCount());
