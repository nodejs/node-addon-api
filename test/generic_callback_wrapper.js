'use strict';
const assert = require('assert');
const buildType = process.config.target_defaults.default_configuration;

module.exports = test(require(`./build/${buildType}/binding.node`));

function test(binding) {
    async function check(bindingFunction) {
        const result = await bindingFunction(5);
        assert.ok(result);
        assert.equal(result.value, 125);
    }


    return check(binding.generic_callback_wrapper.TestWithValuePow3);
}
