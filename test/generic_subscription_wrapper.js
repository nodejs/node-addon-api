'use strict';
const assert = require('assert');
const buildType = process.config.target_defaults.default_configuration;

module.exports = test(require(`./build/${buildType}/binding.node`));

async function test(binding) {
    const tests = [
        async function testNormal10TimesCallAndUnsubscribe(bindingFunction) {
            const functionToTest = bindingFunction.generic_subscription_wrapper.SubScribeFor10Calls;
            assert.doesNotReject(functionToTest(5));
            const result = await functionToTest(5);
            assert.equal(result.value, 125);
            assert.equal(result.error, undefined);
        },
    ];

    for (let f of tests) {
        await f(binding);
    }
}
