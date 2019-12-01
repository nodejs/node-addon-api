'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const testUtil = require('./testUtil');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  testUtil.runGCTests([
    'Plain C string',
    () => {
      const sum = binding.run_script.plainString();
      assert.strictEqual(sum, 1 + 2 + 3);
    },

    'std::string',
    () => {
      const sum = binding.run_script.stdString();
      assert.strictEqual(sum, 1 + 2 + 3);
    },

    'JavaScript string',
    () => {
      const sum = binding.run_script.jsString("1 + 2 + 3");
      assert.strictEqual(sum, 1 + 2 + 3);
    },

    'JavaScript, but not a string',
    () => {
      assert.throws(() => {
        binding.run_script.jsString(true);
      }, {
        name: 'Error',
        message: 'A string was expected'
      });
    },

    'With context',
    () => {
      const a = 1, b = 2, c = 3;
      const sum = binding.run_script.withContext("a + b + c", { a, b, c });
      assert.strictEqual(sum, a + b + c);
    }
  ]);
}
