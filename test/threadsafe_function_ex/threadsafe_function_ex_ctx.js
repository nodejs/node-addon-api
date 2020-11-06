'use strict';

const assert = require('assert');
const buildType = process.config.target_defaults.default_configuration;

module.exports = test(require(`../build/${buildType}/binding.node`))
  .then(() => test(require(`../build/${buildType}/binding_noexcept.node`)));

async function test(binding) {
  const ctx = { };
  const tsfn = new binding.threadsafe_function_ctx.TSFNWrap(ctx);
  assert(tsfn.getContext() === ctx);
  await tsfn.release();
}
