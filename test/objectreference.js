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
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const testUtil = require('./testUtil');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  function testCastedEqual(testToCompare) {
    var compare_test = ["hello", "world", "!"];
    if (testToCompare instanceof Array) {
      assert.deepEqual(compare_test, testToCompare);
    } else if (testToCompare instanceof String) {
      assert.deepEqual("No Referenced Value", testToCompare);
    } else {
      assert.fail();
    }
  }

  testUtil.runGCTests([
    'Weak Casted Array',
    () => {
      binding.objectreference.setCastedObjects();
      var test = binding.objectreference.getCastedFromValue("weak");
      var test2 = new Array();
      test2[0] = binding.objectreference.getCastedFromGetter("weak", 0);
      test2[1] = binding.objectreference.getCastedFromGetter("weak", 1);
      test2[2] = binding.objectreference.getCastedFromGetter("weak", 2);

      testCastedEqual(test);
      testCastedEqual(test2);
    },

    'Persistent Casted Array',
    () => {
      binding.objectreference.setCastedObjects();
      const test = binding.objectreference.getCastedFromValue("persistent");
      const test2 = new Array();
      test2[0] = binding.objectreference.getCastedFromGetter("persistent", 0);
      test2[1] = binding.objectreference.getCastedFromGetter("persistent", 1);
      test2[2] = binding.objectreference.getCastedFromGetter("persistent", 2);

      assert.ok(test instanceof Array);
      assert.ok(test2 instanceof Array);
      testCastedEqual(test);
      testCastedEqual(test2);
    },

    'References Casted Array',
    () => {
      binding.objectreference.setCastedObjects();
      const test = binding.objectreference.getCastedFromValue();
      const test2 = new Array();
      test2[0] = binding.objectreference.getCastedFromGetter("reference", 0);
      test2[1] = binding.objectreference.getCastedFromGetter("reference", 1);
      test2[2] = binding.objectreference.getCastedFromGetter("reference", 2);

      assert.ok(test instanceof Array);
      assert.ok(test2 instanceof Array);
      testCastedEqual(test);
      testCastedEqual(test2);
    },

    'Weak',
    () => {
      binding.objectreference.setObjects("hello", "world");
      const test = binding.objectreference.getFromValue("weak");
      const test2 = binding.objectreference.getFromGetter("weak", "hello");

      assert.deepEqual({ hello: "world"}, test);
      assert.equal("world", test2);
      assert.equal(test["hello"], test2);
    },
    () => {
      binding.objectreference.setObjects("hello", "world", "javascript");
      const test = binding.objectreference.getFromValue("weak");
      const test2 = binding.objectreference.getFromValue("weak", "hello");

      assert.deepEqual({ hello: "world" }, test);
      assert.deepEqual({ hello: "world" }, test2);
      assert.equal(test, test2);
    },
    () => {
      binding.objectreference.setObjects(1, "hello world");
      const test = binding.objectreference.getFromValue("weak");
      const test2 = binding.objectreference.getFromGetter("weak", 1);

      assert.deepEqual({ 1: "hello world" }, test);
      assert.equal("hello world", test2);
      assert.equal(test[1], test2);
    },
    () => {
      binding.objectreference.setObjects(0, "hello");
      binding.objectreference.setObjects(1, "world");
      const test = binding.objectreference.getFromValue("weak");
      const test2 = binding.objectreference.getFromGetter("weak", 0);
      const test3 = binding.objectreference.getFromGetter("weak", 1);

      assert.deepEqual({ 1: "world" }, test);
      assert.equal(undefined, test2);
      assert.equal("world", test3);
    },
    () => {
      binding.objectreference.setObjects("hello", "world");
      assert.doesNotThrow(
        () => {
          var rcount = binding.objectreference.refObjects("weak");
          assert.equal(rcount, 1);
          rcount = binding.objectreference.unrefObjects("weak");
          assert.equal(rcount, 0);
        },
        Error
      );
      assert.throws(
        () => {
          binding.objectreference.unrefObjects("weak");
        },
        Error
      );
    },

    'Persistent',
    () => {
      binding.objectreference.setObjects("hello", "world");
      const test = binding.objectreference.getFromValue("persistent");
      const test2 = binding.objectreference.getFromGetter("persistent", "hello");

      assert.deepEqual({ hello: "world" }, test);
      assert.equal("world", test2);
      assert.equal(test["hello"], test2);
    },
    () => {
      binding.objectreference.setObjects("hello", "world", "javascript");
      const test = binding.objectreference.getFromValue("persistent");
      const test2 = binding.objectreference.getFromValue("persistent", "hello");

      assert.deepEqual({ hello: "world" }, test);
      assert.deepEqual({ hello: "world" }, test2);
      assert.deepEqual(test, test2);
    },
    () => {
      binding.objectreference.setObjects(1, "hello world");
      const test = binding.objectreference.getFromValue("persistent");
      const test2 = binding.objectreference.getFromGetter("persistent", 1);

      assert.deepEqual({ 1: "hello world"}, test);
      assert.equal("hello world", test2);
      assert.equal(test[1], test2);
    },
    () => {
      binding.objectreference.setObjects(0, "hello");
      binding.objectreference.setObjects(1, "world");
      const test = binding.objectreference.getFromValue("persistent");
      const test2 = binding.objectreference.getFromGetter("persistent", 0);
      const test3 = binding.objectreference.getFromGetter("persistent", 1);

      assert.deepEqual({ 1: "world"}, test);
      assert.equal(undefined, test2);
      assert.equal("world", test3);
    },
    () => {
      binding.objectreference.setObjects("hello", "world");
      assert.doesNotThrow(
        () => {
          var rcount = binding.objectreference.unrefObjects("persistent");
          assert.equal(rcount, 0);
          rcount = binding.objectreference.refObjects("persistent");
          assert.equal(rcount, 1);
          rcount = binding.objectreference.unrefObjects("persistent");
          assert.equal(rcount, 0);
          rcount = binding.objectreference.refObjects("persistent");
          assert.equal(rcount, 1);
          rcount = binding.objectreference.unrefObjects("persistent");
          assert.equal(rcount, 0);
        },
        Error
      );
      assert.throws(
        () => {
          binding.objectreference.unrefObjects("persistent");
        },
        Error
      );
    },

    'References',
    () => {
      binding.objectreference.setObjects("hello", "world");
      const test = binding.objectreference.getFromValue();
      const test2 = binding.objectreference.getFromGetter("hello");

      assert.deepEqual({ hello: "world" }, test);
      assert.equal("world", test2);
      assert.equal(test["hello"], test2);
    },
    () => {
      binding.objectreference.setObjects("hello", "world", "javascript");
      const test = binding.objectreference.getFromValue();
      const test2 = binding.objectreference.getFromValue("hello");

      assert.deepEqual({ hello: "world" }, test);
      assert.deepEqual({ hello: "world" }, test2);
      assert.deepEqual(test, test2);
    },
    () => {
      binding.objectreference.setObjects(1, "hello world");
      const test = binding.objectreference.getFromValue();
      const test2 = binding.objectreference.getFromGetter(1);

      assert.deepEqual({ 1: "hello world"}, test);
      assert.equal("hello world", test2);
      assert.equal(test[1], test2);
    },
    () => {
      binding.objectreference.setObjects(0, "hello");
      binding.objectreference.setObjects(1, "world");
      const test = binding.objectreference.getFromValue();
      const test2 = binding.objectreference.getFromGetter(0);
      const test3 = binding.objectreference.getFromGetter(1);

      assert.deepEqual({ 1: "world"}, test);
      assert.equal(undefined, test2);
      assert.equal("world", test3);
    },
    () => {
      binding.objectreference.setObjects("hello", "world");
      assert.doesNotThrow(
        () => {
          var rcount = binding.objectreference.unrefObjects("references");
          assert.equal(rcount, 1);
          rcount = binding.objectreference.refObjects("references");
          assert.equal(rcount, 2);
          rcount = binding.objectreference.unrefObjects("references");
          assert.equal(rcount, 1);
          rcount = binding.objectreference.unrefObjects("references");
          assert.equal(rcount, 0);
        },
        Error
      );
      assert.throws(
        () => {
          binding.objectreference.unrefObjects("references");
        },
        Error
      );
    }
  ])
};
