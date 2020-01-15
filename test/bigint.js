'use strict';

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  const {
    TestInt64,
    TestUint64,
    TestWords,
    IsLossless,
    TestTooBigBigInt,
  } = binding.bigint;

  [
    0n,
    -0n,
    1n,
    -1n,
    100n,
    2121n,
    -1233n,
    986583n,
    -976675n,
    98765432213456789876546896323445679887645323232436587988766545658n,
    -4350987086545760976737453646576078997096876957864353245245769809n,
  ].forEach((num) => {
    if (num > -(2n ** 63n) && num < 2n ** 63n) {
      assert.strictEqual(TestInt64(num), num);
      assert.strictEqual(IsLossless(num, true), true);
    } else {
      assert.strictEqual(IsLossless(num, true), false);
    }

    if (num >= 0 && num < 2n ** 64n) {
      assert.strictEqual(TestUint64(num), num);
      assert.strictEqual(IsLossless(num, false), true);
    } else {
      assert.strictEqual(IsLossless(num, false), false);
    }

    assert.strictEqual(num, TestWords(num));
  });

  assert.throws(TestTooBigBigInt, {
    name: /^(RangeError|Error)$/,
    message: /^(Maximum BigInt size exceeded|Invalid argument)$/,
  });
}
