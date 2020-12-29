'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const testUtil = require('./testUtil');

function test(binding) {
  return testUtil.runGCTests([
    'objectwrap ref',
    () => {
      const { ObjectWrapRef } = binding.objectwrap_ref;
      const objectWrapRef = new ObjectWrapRef();
      assert.equal(objectWrapRef.ref(), 1);
      // assert.equal(objectWrapRef.unref(), 0);
    },
    // Do on gc before returning.
    () => {}
  ]);
}

module.exports = test(require(`./build/${buildType}/binding.node`))
  .then(() => test(require(`./build/${buildType}/binding_noexcept.node`)));
