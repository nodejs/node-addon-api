'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

const test = (binding) => {
  const Test = binding.objectwrap.Test;

  const testValue = (obj, clazz) => {
    assert.strictEqual(obj.testValue, true);
    assert.strictEqual(obj[clazz.kTestValueInternal], false);
  };

  const testAccessor = (obj, clazz) => {
    // read-only, write-only
    {
      obj.testSetter = 'instance getter';
      assert.strictEqual(obj.testGetter, 'instance getter');

      obj.testSetter = 'instance getter 2';
      assert.strictEqual(obj.testGetter, 'instance getter 2');
    }

    // read write-only
    {
      let error;
      try { const read = obj.testSetter; } catch (e) { error = e; }
      // no error
      assert.strictEqual(error, undefined);

      // read is undefined
      assert.strictEqual(obj.testSetter, undefined);
    }

    // write read-only
    {
      let error;
      try { obj.testGetter = 'write'; } catch (e) { error = e; }
      assert.strictEqual(error.name, 'TypeError');
    }

    // rw
    {
      obj.testGetSet = 'instance getset';
      assert.strictEqual(obj.testGetSet, 'instance getset');

      obj.testGetSet = 'instance getset 2';
      assert.strictEqual(obj.testGetSet, 'instance getset 2');
    }

    // rw symbol
    {
      obj[clazz.kTestAccessorInternal] = 'instance internal getset';
      assert.strictEqual(obj[clazz.kTestAccessorInternal], 'instance internal getset');

      obj[clazz.kTestAccessorInternal] = 'instance internal getset 2';
      assert.strictEqual(obj[clazz.kTestAccessorInternal], 'instance internal getset 2');
    }
  };

  const testMethod = (obj, clazz) => {
    assert.strictEqual(obj.testMethod('method'), 'method instance');
    assert.strictEqual(obj[clazz.kTestMethodInternal]('method'), 'method instance internal');
  };

  const testEnumerables = (obj, clazz) => {
    // Object.keys: only object
    assert.deepEqual(Object.keys(obj), []);

    // for..in: object + prototype
    {
      const keys = [];
      for (let key in obj) {
        keys.push(key);
      }

      assert(keys.length = 4);
      assert(obj.testGetSet);
      assert(obj.testGetter);
      assert(obj.testValue);
      assert(obj.testMethod);
    }
  };

  const testConventions = (obj, clazz) => {
    // test @@toStringTag
    {
      assert.strictEqual(obj[Symbol.toStringTag], 'TestTag');
      assert.strictEqual('' + obj, '[object TestTag]');
    }

    // test @@iterator
    {
      obj.testSetter = 'iterator';
      const values = [];

      for (let item of obj) {
        values.push(item);
      }

      assert.deepEqual(values, ['iterator']);
    }
  };

  const testStaticValue = (clazz) => {
    assert.strictEqual(clazz.testStaticValue, 'value');
    assert.strictEqual(clazz[clazz.kTestStaticValueInternal], 5);
  }

  const testStaticAccessor = (clazz) => {
    // read-only, write-only
    {
      const tempObj = {};
      clazz.testStaticSetter = tempObj;
      assert.strictEqual(clazz.testStaticGetter, tempObj);

      const tempArray = [];
      clazz.testStaticSetter = tempArray;
      assert.strictEqual(clazz.testStaticGetter, tempArray);
    }

    // read write-only
    {
      let error;
      try { const read = clazz.testStaticSetter; } catch (e) { error = e; }
      // no error
      assert.strictEqual(error, undefined);

      // read is undefined
      assert.strictEqual(clazz.testStaticSetter, undefined);
    }

    // write-read-only
    {
      let error;
      try { clazz.testStaticGetter = 'write'; } catch (e) { error = e; }
      assert.strictEqual(error.name, 'TypeError');
    }

    // rw
    {
      clazz.testStaticGetSet = 9;
      assert.strictEqual(clazz.testStaticGetSet, 9);

      clazz.testStaticGetSet = 4;
      assert.strictEqual(clazz.testStaticGetSet, 4);
    }

    // rw symbol
    {
      clazz[clazz.kTestStaticAccessorInternal] = 'static internal getset';
      assert.strictEqual(clazz[clazz.kTestStaticAccessorInternal], 'static internal getset');
    }
  };

  const testStaticMethod = (clazz) => {
    assert.strictEqual(clazz.testStaticMethod('method'), 'method static');
    assert.strictEqual(clazz[clazz.kTestStaticMethodInternal]('method'), 'method static internal');
  };

  const testStaticEnumerables = (clazz) => {
    // Object.keys
    assert.deepEqual(Object.keys(clazz), [
      'testStaticValue',
      'testStaticGetter',
      'testStaticGetSet',
      'testStaticMethod'
    ]);

    // for..in
    {
      const keys = [];
      for (let key in clazz) {
        keys.push(key);
      }

      assert.deepEqual(keys, [
        'testStaticValue',
        'testStaticGetter',
        'testStaticGetSet',
        'testStaticMethod'
      ]);
    }
  };

  const testFinalize = (clazz) => {

    let finalizeCalled = false;
    const finalizeCb = function(called) {
      finalizeCalled = called;
    };

    //Scope Test instance so that it can be gc'd.
    (function() {
      new Test(finalizeCb);
    })();

    global.gc();

    assert.strictEqual(finalizeCalled, true);

  };

  const testObj = (obj, clazz) => {
    testValue(obj, clazz);
    testAccessor(obj, clazz);
    testMethod(obj, clazz);

    testEnumerables(obj, clazz);

    testConventions(obj, clazz);
  }

  const testClass = (clazz) => {
    testStaticValue(clazz);
    testStaticAccessor(clazz);
    testStaticMethod(clazz);

    testStaticEnumerables(clazz);
    testFinalize(clazz);
  };

  // `Test` is needed for accessing exposed symbols
  testObj(new Test(), Test);
  testClass(Test);

  // Make sure the C++ object can be garbage collected without issues.
  setImmediate(global.gc);
}

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));
