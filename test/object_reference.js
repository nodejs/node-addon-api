/*
 * First tests are for setting and getting the ObjectReference on the
 * casted Array as Object. Then the tests are for the ObjectReference
 * to an empty Object. They test setting the ObjectReference with a C
 * string, a JavaScript string, and a JavaScript Number as the keys.
 * Then getting the value of those keys through the Reference function
 * Value() and through the ObjectReference getters. Finally, they test
 * Unref() and Ref() to determine if the reference count is as
 * expected and errors are thrown when expected.
 */

'use strict';

const assert = require('assert');
const testUtil = require('./testUtil');

module.exports = require('./common').runTest(test);

const enumType = {
  JS: 0,
  C_STR: 1,
  BOOL: 2,
  INT: 3,
  DOUBLE: 4,
  JS_CAST: 5
};

function test (binding) {
  function testCastedEqual (testToCompare) {
    const compareTest = ['hello', 'world', '!'];
    if (testToCompare instanceof Array) {
      assert.deepEqual(compareTest, testToCompare);
    } else if (testToCompare instanceof String) {
      assert.deepEqual('No Referenced Value', testToCompare);
    } else {
      assert.fail();
    }
  }

  return testUtil.runGCTests([
    'Weak Casted Array',
    () => {
      binding.objectreference.setCastedObjects();
      const test = binding.objectreference.getCastedFromValue('weak');
      const test2 = [];
      test2[0] = binding.objectreference.getCastedFromGetter('weak', 0);
      test2[1] = binding.objectreference.getCastedFromGetter('weak', 1);
      test2[2] = binding.objectreference.getCastedFromGetter('weak', 2);

      testCastedEqual(test);
      testCastedEqual(test2);
    },

    'Persistent Casted Array',
    () => {
      binding.objectreference.setCastedObjects();
      const test = binding.objectreference.getCastedFromValue('persistent');
      const test2 = [];
      test2[0] = binding.objectreference.getCastedFromGetter('persistent', 0);
      test2[1] = binding.objectreference.getCastedFromGetter('persistent', 1);
      test2[2] = binding.objectreference.getCastedFromGetter('persistent', 2);

      assert.ok(test instanceof Array);
      assert.ok(test2 instanceof Array);
      testCastedEqual(test);
      testCastedEqual(test2);
    },

    'References Casted Array',
    () => {
      binding.objectreference.setCastedObjects();
      const test = binding.objectreference.getCastedFromValue();
      const test2 = [];
      test2[0] = binding.objectreference.getCastedFromGetter('reference', 0);
      test2[1] = binding.objectreference.getCastedFromGetter('reference', 1);
      test2[2] = binding.objectreference.getCastedFromGetter('reference', 2);

      assert.ok(test instanceof Array);
      assert.ok(test2 instanceof Array);
      testCastedEqual(test);
      testCastedEqual(test2);
    },

    'Weak',
    () => {
      const configObjects = [
        { keyType: enumType.C_STR, valType: enumType.JS, key: 'hello', val: 'world' },
        { keyType: enumType.C_STR, valType: enumType.C_STR, key: 'hello', val: 'world' },
        { keyType: enumType.C_STR, valType: enumType.BOOL, key: 'hello', val: false },
        { keyType: enumType.C_STR, valType: enumType.DOUBLE, key: 'hello', val: 3.56 },
        { keyType: enumType.C_STR, valType: enumType.JS_CAST, key: 'hello_cast', val: 'world' },
        { keyType: enumType.JS, valType: enumType.JS, key: 'hello_js', val: 'world_js' },
        { keyType: enumType.JS, valType: enumType.JS_CAST, key: 'hello_js', val: 'world_js' },
        { keyType: enumType.JS, valType: enumType.BOOL, key: 'hello_js', val: true },
        { keyType: enumType.INT, valType: enumType.JS, key: 1, val: 'hello world' },
        { keyType: enumType.INT, valType: enumType.JS_CAST, key: 2, val: 'hello world' },
        { keyType: enumType.INT, valType: enumType.C_STR, key: 3, val: 'hello world' },
        { keyType: enumType.INT, valType: enumType.BOOL, key: 3, val: false },
        { keyType: enumType.INT, valType: enumType.DOUBLE, key: 4, val: 3.14159 }
      ];

      for (const configObject of configObjects) {
        binding.objectreference.setObject(configObject);
        const test = binding.objectreference.getFromValue('weak');
        const test2 = binding.objectreference.getFromGetter('weak', configObject.key);

        const assertObject = {
          [configObject.key]: configObject.val
        };
        assert.deepEqual(assertObject, test);
        assert.equal(configObject.val, test2);
      }
    }, () => {
      binding.objectreference.setObject({ keyType: enumType.INT, valType: enumType.JS, key: 0, val: 'hello' });
      binding.objectreference.setObject({ keyType: enumType.INT, valType: enumType.JS, key: 1, val: 'world' });

      const test = binding.objectreference.getFromValue('weak');
      const test2 = binding.objectreference.getFromGetter('weak', 0);
      const test3 = binding.objectreference.getFromGetter('weak', 1);
      assert.deepEqual({ 1: 'world' }, test);
      assert.equal(undefined, test2);
      assert.equal('world', test3);
    },
    () => {
      binding.objectreference.setObject({ keyType: enumType.JS, valType: enumType.JS, key: 'hello', val: 'world' });
      assert.doesNotThrow(
        () => {
          let rcount = binding.objectreference.refObjects('weak');
          assert.equal(rcount, 1);
          rcount = binding.objectreference.unrefObjects('weak');
          assert.equal(rcount, 0);
        },
        Error
      );
      assert.throws(
        () => {
          binding.objectreference.unrefObjects('weak');
        },
        Error
      );
    },

    'Persistent',
    () => {
      binding.objectreference.setObjects('hello', 'world');
      const test = binding.objectreference.getFromValue('persistent');
      const test2 = binding.objectreference.getFromGetter('persistent', 'hello');

      assert.deepEqual({ hello: 'world' }, test);
      assert.equal('world', test2);
      assert.equal(test.hello, test2);
    },
    () => {
      binding.objectreference.setObjects('hello', 'world', 'javascript');
      const test = binding.objectreference.getFromValue('persistent');
      const test2 = binding.objectreference.getFromValue('persistent', 'hello');

      assert.deepEqual({ hello: 'world' }, test);
      assert.deepEqual({ hello: 'world' }, test2);
      assert.deepEqual(test, test2);
    },
    () => {
      binding.objectreference.setObjects(1, 'hello world');
      const test = binding.objectreference.getFromValue('persistent');
      const test2 = binding.objectreference.getFromGetter('persistent', 1);

      assert.deepEqual({ 1: 'hello world' }, test);
      assert.equal('hello world', test2);
      assert.equal(test[1], test2);
    },
    () => {
      binding.objectreference.setObjects(0, 'hello');
      binding.objectreference.setObjects(1, 'world');
      const test = binding.objectreference.getFromValue('persistent');
      const test2 = binding.objectreference.getFromGetter('persistent', 0);
      const test3 = binding.objectreference.getFromGetter('persistent', 1);

      assert.deepEqual({ 1: 'world' }, test);
      assert.equal(undefined, test2);
      assert.equal('world', test3);
    },
    () => {
      binding.objectreference.setObjects('hello', 'world');
      assert.doesNotThrow(
        () => {
          let rcount = binding.objectreference.unrefObjects('persistent');
          assert.equal(rcount, 0);
          rcount = binding.objectreference.refObjects('persistent');
          assert.equal(rcount, 1);
          rcount = binding.objectreference.unrefObjects('persistent');
          assert.equal(rcount, 0);
          rcount = binding.objectreference.refObjects('persistent');
          assert.equal(rcount, 1);
          rcount = binding.objectreference.unrefObjects('persistent');
          assert.equal(rcount, 0);
        },
        Error
      );
      assert.throws(
        () => {
          binding.objectreference.unrefObjects('persistent');
        },
        Error
      );
    },

    'References',
    () => {
      binding.objectreference.setObjects('hello', 'world');
      const test = binding.objectreference.getFromValue();
      const test2 = binding.objectreference.getFromGetter('hello');

      assert.deepEqual({ hello: 'world' }, test);
      assert.equal('world', test2);
      assert.equal(test.hello, test2);
    },
    () => {
      binding.objectreference.setObjects('hello', 'world', 'javascript');
      const test = binding.objectreference.getFromValue();
      const test2 = binding.objectreference.getFromValue('hello');

      assert.deepEqual({ hello: 'world' }, test);
      assert.deepEqual({ hello: 'world' }, test2);
      assert.deepEqual(test, test2);
    },
    () => {
      binding.objectreference.setObjects(1, 'hello world');
      const test = binding.objectreference.getFromValue();
      const test2 = binding.objectreference.getFromGetter(1);

      assert.deepEqual({ 1: 'hello world' }, test);
      assert.equal('hello world', test2);
      assert.equal(test[1], test2);
    },
    () => {
      binding.objectreference.setObjects(0, 'hello');
      binding.objectreference.setObjects(1, 'world');
      const test = binding.objectreference.getFromValue();
      const test2 = binding.objectreference.getFromGetter(0);
      const test3 = binding.objectreference.getFromGetter(1);

      assert.deepEqual({ 1: 'world' }, test);
      assert.equal(undefined, test2);
      assert.equal('world', test3);
    },
    () => {
      binding.objectreference.setObjects('hello', 'world');
      assert.doesNotThrow(
        () => {
          let rcount = binding.objectreference.unrefObjects('references');
          assert.equal(rcount, 1);
          rcount = binding.objectreference.refObjects('references');
          assert.equal(rcount, 2);
          rcount = binding.objectreference.unrefObjects('references');
          assert.equal(rcount, 1);
          rcount = binding.objectreference.unrefObjects('references');
          assert.equal(rcount, 0);
        },
        Error
      );
      assert.throws(
        () => {
          binding.objectreference.unrefObjects('references');
        },
        Error
      );
    }
  ]);
}
