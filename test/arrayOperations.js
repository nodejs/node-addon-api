'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const testUtil = require('./testUtil');

module.exports = test(require(`./build/${buildType}/binding.node`))
  .then(() => test(require(`./build/${buildType}/binding_noexcept.node`)));

function test(binding) {
  return testUtil.runGCTests([
    'display array',
    ()=>{
        let arr=[2,3];
        assert.strictEqual(binding.arrayoperations.ShowValue(arr),arr);
    },
    'add constant to array',
    ()=>{
        let arr=[2,3];
        let charr=[4,5];
        let result = binding.arrayoperations.addby(arr,2);
        for(var i=0;i<arr.length;i++){
            assert.strictEqual(result[i],charr[i]);

        }

    },
  ]);
}