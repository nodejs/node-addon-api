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

    'Weak Dummy',
    () => {
      binding.objectreference.setDummyObjects("hello", "world");
      const test = binding.objectreference.getDummyFromValue("weak");
      const test2 = binding.objectreference.getDummyFromGetter("weak", "hello");

      assert.deepEqual({ hello: "world"}, test);
      assert.equal("world", test2);
      assert.equal(test["hello"], test2);
    },
    () => {
      binding.objectreference.setDummyObjects(1, "hello world");
      const test = binding.objectreference.getDummyFromValue("weak");
      const test2 = binding.objectreference.getDummyFromGetter("weak", 1);

      assert.deepEqual({ 1: "hello world"}, test);
      assert.equal("hello world", test2);
      assert.equal(test[1], test2);
    },
    () => {
      binding.objectreference.setDummyObjects(0, "hello");
      binding.objectreference.setDummyObjects(1, "world");
      const test = binding.objectreference.getDummyFromValue("weak");
      const test2 = binding.objectreference.getDummyFromGetter("weak", 0);
      const test3 = binding.objectreference.getDummyFromGetter("weak", 1);

      assert.deepEqual({ 1: "world"}, test);
      assert.equal(undefined, test2);
      assert.equal("world", test3);
    },

    'Persistent Dummy',
    () => {
      binding.objectreference.setDummyObjects("hello", "world");
      const test = binding.objectreference.getDummyFromValue("persistent");
      const test2 = binding.objectreference.getDummyFromGetter("persistent", "hello");

      assert.deepEqual({ hello: "world"}, test);
      assert.equal("world", test2);
      assert.equal(test["hello"], test2);
    },
    () => {
      binding.objectreference.setDummyObjects(1, "hello world");
      const test = binding.objectreference.getDummyFromValue("persistent");
      const test2 = binding.objectreference.getDummyFromGetter("persistent", 1);

      assert.deepEqual({ 1: "hello world"}, test);
      assert.equal("hello world", test2);
      assert.equal(test[1], test2);
    },
    () => {
      binding.objectreference.setDummyObjects(0, "hello");
      binding.objectreference.setDummyObjects(1, "world");
      const test = binding.objectreference.getDummyFromValue("persistent");
      const test2 = binding.objectreference.getDummyFromGetter("persistent", 0);
      const test3 = binding.objectreference.getDummyFromGetter("persistent", 1);

      assert.deepEqual({ 1: "world"}, test);
      assert.equal(undefined, test2);
      assert.equal("world", test3);
    },
    () => {
      binding.objectreference.setDummyObjects("hello", "world");
      assert.doesNotThrow(
        () => {
          binding.objectreference.unrefObjects("dummy persistent");
        },
        Error
      );
      assert.throws(
        () => {
          binding.objectreference.unrefObjects("dummy persistent");
        },
        Error
      );
    },

    'References Dummy',
    () => {
      binding.objectreference.setDummyObjects("hello", "world");
      const test = binding.objectreference.getDummyFromValue();
      const test2 = binding.objectreference.getDummyFromGetter("hello");

      assert.deepEqual({ hello: "world"}, test);
      assert.equal("world", test2);
      assert.equal(test["hello"], test2);
    },
    () => {
      binding.objectreference.setDummyObjects(1, "hello world");
      const test = binding.objectreference.getDummyFromValue();
      const test2 = binding.objectreference.getDummyFromGetter(1);

      assert.deepEqual({ 1: "hello world"}, test);
      assert.equal("hello world", test2);
      assert.equal(test[1], test2);
    },
    () => {
      binding.objectreference.setDummyObjects(0, "hello");
      binding.objectreference.setDummyObjects(1, "world");
      const test = binding.objectreference.getDummyFromValue();
      const test2 = binding.objectreference.getDummyFromGetter(0);
      const test3 = binding.objectreference.getDummyFromGetter(1);

      assert.deepEqual({ 1: "world"}, test);
      assert.equal(undefined, test2);
      assert.equal("world", test3);
    },
    () => {
      binding.objectreference.setDummyObjects("hello", "world");
      assert.doesNotThrow(
        () => {
          binding.objectreference.unrefObjects("dummy reference");
        },
        Error
      );
      assert.doesNotThrow(
        () => {
          binding.objectreference.unrefObjects("dummy reference");
        },
        Error
      );
      assert.throws(
        () => {
          binding.objectreference.unrefObjects("dummy reference");
        },
        Error
      );
    }
  ])
};
