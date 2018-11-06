'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  const jsonStringify = binding.json.jsonStringify;
  const jsonParse = binding.json.jsonParse;
  // Tests for JSON.parse
  assert.deepStrictEqual(jsonParse('{ "a": "JSON", "string": "value" }'),
    JSON.parse('{ "a": "JSON", "string": "value" }'));
  assert.deepStrictEqual(jsonParse('[ 1, 2, 3 ]'), JSON.parse('[ 1, 2, 3 ]'));
  assert.deepStrictEqual(jsonParse('57'), JSON.parse('57'));
  assert.deepStrictEqual(jsonParse('3.14159'), JSON.parse('3.14159'));
  assert.deepStrictEqual(jsonParse('true'), JSON.parse('true'));
  assert.deepStrictEqual(jsonParse('false'), JSON.parse('false'));
  assert.deepStrictEqual(jsonParse('"some string"'), JSON.parse('"some string"'));
  // Tests for JSON.stringify
  assert.deepStrictEqual(jsonStringify({ "a": "JSON", "object": "value" }),
    JSON.stringify({ "a": "JSON", "object": "value" }));
  assert.deepStrictEqual(jsonStringify({ "a": "JSON", "object": "value" }, null, 2),
    JSON.stringify({ "a": "JSON", "object": "value" }, null, 2));
  assert.deepStrictEqual(jsonStringify({ "a": "JSON", "object": "value" }, null, '++'),
    JSON.stringify({ "a": "JSON", "object": "value" }, null, '++'));
  assert.deepStrictEqual(jsonStringify([ 1, 2, 3 ]), JSON.stringify([ 1, 2, 3 ]));
  assert.deepStrictEqual(jsonStringify([ 1, 2, 3 ], null, 2),
    JSON.stringify([ 1, 2, 3 ], null, 2));
  assert.deepStrictEqual(jsonStringify([ 1, 2, 3 ], null, '++'),
    JSON.stringify([ 1, 2, 3 ], null, '++'));
  assert.deepStrictEqual(jsonStringify("a string"),
    JSON.stringify("a string"));
  assert.deepStrictEqual(jsonStringify("a string", null, 2),
    JSON.stringify("a string", null, 2));
  assert.deepStrictEqual(jsonStringify("a string", null, '++'),
    JSON.stringify("a string", null, '++'));
  assert.deepStrictEqual(jsonStringify(3.14159), JSON.stringify(3.14159));
  assert.deepStrictEqual(jsonStringify(3.14159, null, 2),
    JSON.stringify(3.14159, null, 2));
  assert.deepStrictEqual(jsonStringify(3.14159, null, '++'),
    JSON.stringify(3.14159, null, '++'));
  assert.deepStrictEqual(jsonStringify(-31), JSON.stringify(-31));
  assert.deepStrictEqual(jsonStringify(-31, null, 2),
    JSON.stringify(-31, null, 2));
  assert.deepStrictEqual(jsonStringify(-31, null, '++'),
    JSON.stringify(-31, null, '++'));
  assert.deepStrictEqual(jsonStringify(true),JSON.stringify(true));
  assert.deepStrictEqual(jsonStringify(true, null, 2),
    JSON.stringify(true, null, 2));
  assert.deepStrictEqual(jsonStringify(true, null, '++'),
    JSON.stringify(true, null, '++'));
  assert.deepStrictEqual(jsonStringify(false), JSON.stringify(false));
  assert.deepStrictEqual(jsonStringify(false, null, 2),
    JSON.stringify(false, null, 2));
  assert.deepStrictEqual(jsonStringify(false, null, '++'),
    JSON.stringify(false, null, '++'));

}
