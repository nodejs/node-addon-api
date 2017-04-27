'use strict';
const buildType = process.config.target_defaults.default_configuration;
const binding = require(`./build/${buildType}/binding.node`);
const assert = require('assert');

const expected = '123456789';

assert.ok(binding.name.checkString(expected, 'utf8'));
assert.ok(binding.name.checkString(expected, 'utf16'));
assert.ok(binding.name.checkString(expected.substr(0, 3), 'utf8', 3));
assert.ok(binding.name.checkString(expected.substr(0, 3), 'utf16', 3));

const str1 = binding.name.createString('utf8');
assert.strictEqual(str1, expected);
assert.ok(binding.name.checkString(str1, 'utf8'));
assert.ok(binding.name.checkString(str1, 'utf16'));

const substr1 = binding.name.createString('utf8', 3);
assert.strictEqual(substr1, expected.substr(0, 3));
assert.ok(binding.name.checkString(substr1, 'utf8', 3));
assert.ok(binding.name.checkString(substr1, 'utf16', 3));

const str2 = binding.name.createString('utf16');
assert.strictEqual(str1, expected);
assert.ok(binding.name.checkString(str2, 'utf8'));
assert.ok(binding.name.checkString(str2, 'utf16'));

const substr2 = binding.name.createString('utf16', 3);
assert.strictEqual(substr1, expected.substr(0, 3));
assert.ok(binding.name.checkString(substr2, 'utf8', 3));
assert.ok(binding.name.checkString(substr2, 'utf16', 3));

assert.ok(binding.name.checkSymbol(Symbol()));
assert.ok(binding.name.checkSymbol(Symbol('test')));

const sym1 = binding.name.createSymbol();
assert.strictEqual(typeof sym1, 'symbol');
assert.ok(binding.name.checkSymbol(sym1));

const sym2 = binding.name.createSymbol('test');
assert.strictEqual(typeof sym2, 'symbol');
assert.ok(binding.name.checkSymbol(sym1));
