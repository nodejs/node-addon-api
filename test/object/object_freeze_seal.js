'use strict';

const assert = require('assert');

module.exports = require('../common').runTest(test);

function test(binding) {
    {
        const obj = { x: 'a', y: 'b', z: 'c' };
        binding.object_freeze_seal.freeze(obj);
        assert.strictEqual(Object.isFrozen(obj), true);
        assert.throws(() => {
          obj.x = 10;
        }, /Cannot assign to read only property 'x' of object '#<Object>/);
        assert.throws(() => {
          obj.w = 15;
        }, /Cannot add property w, object is not extensible/);
        assert.throws(() => {
          delete obj.x;
        }, /Cannot delete property 'x' of #<Object>/);
    }

    {
        const obj = { x: 'a', y: 'b', z: 'c' };
        binding.object_freeze_seal.seal(obj);
        assert.strictEqual(Object.isSealed(obj), true);
        assert.throws(() => {
          obj.w = 'd';
        }, /Cannot add property w, object is not extensible/);
        assert.throws(() => {
          delete obj.x;
        }, /Cannot delete property 'x' of #<Object>/);
        // Sealed objects allow updating existing properties,
        // so this should not throw.
        obj.x = 'd';
    }
}
