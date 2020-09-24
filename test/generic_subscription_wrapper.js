'use strict';
const assert = require('assert');
const buildType = process.config.target_defaults.default_configuration;

module.exports = test(require(`./build/${buildType}/binding.node`));

async function testNormal10TimesCallAndUnsubscribe(bindingFunction) {
    class Handler {
        constructor(finish) {
            this.countsLeft = 10;
            this.callCounter = 0;
            this.finish = finish;
            this.checkSum = 0;
        }
        iteration(input) {
            ++this.callCounter;
            if (--this.countsLeft === 0) {
                this.finish();
            }
            this.checkSum += input;
        }
    }

    const functionToTest = bindingFunction.generic_subscription_wrapper.SubScribeFor10Calls;

    let handler = null; let unsub = null;
    let p = new Promise(resolve => {
        handler = new Handler(() => resolve());
        unsub = functionToTest((input) => handler.iteration(input))
    });

    await p;
    assert.equal(handler.callCounter, 10);
    assert.equal(handler.checkSum, 45);
    await unsub.then(u => (u()));

}

async function test(binding) {
    const tests = [
        testNormal10TimesCallAndUnsubscribe,
    ];

    for (let f of tests) {
        await f(binding);
    }
}
