'use strict';
const buildType = process.config.target_defaults.default_configuration;
const binding = require(`./build/${buildType}/binding.node`);
const assert = require('assert');

assert.strictEqual(0, binding.external.getFinalizeCount());

let test = binding.external.createExternal();
assert.strictEqual(typeof test, 'object');
binding.external.checkExternal(test);
test = null;
global.gc();
assert.strictEqual(0, binding.external.getFinalizeCount());

test = binding.external.createExternalWithFinalize();
assert.strictEqual(typeof test, 'object');
binding.external.checkExternal(test);
test = null;
global.gc();
assert.strictEqual(1, binding.external.getFinalizeCount());

test = binding.external.createExternalWithFinalizeHint();
assert.strictEqual(typeof test, 'object');
binding.external.checkExternal(test);
test = null;
global.gc();
assert.strictEqual(2, binding.external.getFinalizeCount());
