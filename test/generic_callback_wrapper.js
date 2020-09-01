'use strict';
const assert = require('assert');
const buildType = process.config.target_defaults.default_configuration;

module.exports = test(require(`./build/${buildType}/binding.node`));

async function test(binding) {
    const tests = [
        async function testNormalFunctionCallErrorCodeVersion(bindingFunction) {
            const functionToTest = bindingFunction.generic_callback_wrapper.TestPow3ErrorValue;
            assert.doesNotReject(functionToTest(5));
            const result = await functionToTest(5);
            assert.equal(result.value, 125);
            assert.equal(result.error, undefined);
        },
        async function testErrorCodeFunctionCall(bindingFunction) {
            const functionToTest = bindingFunction.generic_callback_wrapper.TestPow3ErrorValue;
            assert.doesNotReject(functionToTest(42));
            const result = await functionToTest(42);
            assert.equal(result.value, undefined);
            assert.notEqual(result.error, undefined);
            assert.notEqual(result.error, null);
        },
        async function testNormalFunctionCallThrowingVersion(bindingFunction) {
            const functionToTest = bindingFunction.generic_callback_wrapper.TestPow3Throwing;
            assert.doesNotReject(functionToTest(5));
            const result = await functionToTest(5);
            assert.equal(result, 125);
        },
        async function testThrowingFunctionCall(bindingFunction) {
            const functionToTest = bindingFunction.generic_callback_wrapper.TestPow3Throwing;
            assert.rejects(functionToTest(42));
        },
    ];

    for (let f of tests) {
        await f(binding);
    }
}
