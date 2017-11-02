'use strict';
const assert = require('assert');

if (process.argv[2] === 'fatal') {
  const binding = require('./load-bindings')(process.argv[3] === 'true' ?
    'binding' : 'binding_noexcept');
  binding.error.throwFatalError();
  return;
}

test(require('./load-bindings')('binding'), true);
test(require('./load-bindings')('binding_noexcept'), false);

function test(binding, withExceptions) {

  assert.throws(() => binding.error.throwApiError('test'), function(err) {
    return err instanceof Error && err.message.includes('Invalid');
  });

  assert.throws(() => binding.error.throwJSError('test'), function(err) {
    return err instanceof Error && err.message === 'test';
  });

  assert.throws(() => binding.error.throwTypeError('test'), function(err) {
    return err instanceof TypeError && err.message === 'test';
  });

  assert.throws(() => binding.error.throwRangeError('test'), function(err) {
    return err instanceof RangeError && err.message === 'test';
  });

  assert.throws(
    () => binding.error.doNotCatch(
      () => {
        throw new TypeError('test');
      }),
    function(err) {
      return err instanceof TypeError && err.message === 'test' && !err.caught;
    });

  assert.throws(
    () => binding.error.catchAndRethrowError(
      () => {
        throw new TypeError('test');
      }),
    function(err) {
      return err instanceof TypeError && err.message === 'test' && err.caught;
    });

  const err = binding.error.catchError(
    () => { throw new TypeError('test'); });
  assert(err instanceof TypeError);
  assert.strictEqual(err.message, 'test');

  const msg = binding.error.catchErrorMessage(
    () => { throw new TypeError('test'); });
  assert.strictEqual(msg, 'test');

  assert.throws(() => binding.error.throwErrorThatEscapesScope('test'), function(err) {
    return err instanceof Error && err.message === 'test';
  });

  assert.throws(() => binding.error.catchAndRethrowErrorThatEscapesScope('test'), function(err) {
    return err instanceof Error && err.message === 'test' && err.caught;
  });

  const p = require('./napi_child').spawnSync(
      process.execPath, [ __filename, 'fatal', "" + withExceptions ]);
  assert.ifError(p.error);
  assert.ok(p.stderr.toString().includes(
      'FATAL ERROR: Error::ThrowFatalError This is a fatal error'));
}
