'use strict';
const binding = require('./build/Release/binding.node');
const assert = require('assert');

assert.deepStrictEqual(binding.test1(), { "foo": "bar" });
