'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

if (process.argv[2] === 'fatal') {
  const binding = require(process.argv[3]);
  binding.error.throwFatalError();
  return;
}

test(`./build/${buildType}/binding.node`);
test(`./build/${buildType}/binding_noexcept.node`);

function test(bindingPath) {
  const binding = require(bindingPath);

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
      process.execPath, [ __filename, 'fatal', bindingPath ]);
  assert.ifError(p.error);
  assert.ok(p.stderr.toString().includes(
      'FATAL ERROR: Error::ThrowFatalError This is a fatal error'));
}
