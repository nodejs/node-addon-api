'use strict';

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

const napiChild = require('../napi_child');

module.exports = test(require(`../build/${buildType}/binding_noexcept_maybe.node`).maybe_check);

function test (binding) {
  if (process.argv.includes('child')) {
    child(binding);
    return;
  }
  const cp = napiChild.spawn(process.execPath, [__filename, 'child'], {
    stdio: ['ignore', 'inherit', 'pipe']
  });
  cp.stderr.setEncoding('utf8');
  let stderr = '';
  cp.stderr.on('data', chunk => {
    stderr += chunk;
  });
  cp.on('exit', (code, signal) => {
    if (process.platform === 'win32') {
      assert.strictEqual(code, 128 + 6 /* SIGABRT */);
    } else {
      assert.strictEqual(signal, 'SIGABRT');
    }
    assert.ok(stderr.match(/FATAL ERROR: Napi::Maybe::Check Maybe value is Nothing./));
  });
}

function child (binding) {
  binding.voidCallback(() => {
    throw new Error('foobar');
  });
}
