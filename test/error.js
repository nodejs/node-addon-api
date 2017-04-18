'use strict';
const buildType = process.config.target_defaults.default_configuration;
const binding = require(`./build/${buildType}/binding.node`);
const assert = require('assert');

assert.throws(() => binding.error.throwError('test'), err => {
   return err instanceof Error && err.message === 'test';
});

assert.throws(() => binding.error.throwTypeError('test'), err => {
   return err instanceof TypeError && err.message === 'test';
});

assert.throws(() => binding.error.throwRangeError('test'), err => {
   return err instanceof RangeError && err.message === 'test';
});

assert.throws(
  () => binding.error.doNotCatch(
    () => {
      throw new TypeError('test');
    }),
  err => {
    return err instanceof TypeError && err.message === 'test' && !err.caught;
  });

assert.throws(
  () => binding.error.catchAndRethrowError(
    () => {
      throw new TypeError('test');
    }),
  err => {
    return err instanceof TypeError && err.message === 'test' && err.caught;
  });

const err = binding.error.catchError(
   () => { throw new TypeError('test'); });
assert(err instanceof TypeError);
assert.strictEqual(err.message, 'test');

const msg = binding.error.catchErrorMessage(
   () => { throw new TypeError('test'); });
assert.strictEqual(msg, 'test');
