'use strict';
const assert = require('assert');
const buildType = process.config.target_defaults.default_configuration;

module.exports = test(require(`./build/${buildType}/binding.node`));

async function test(binding) {
    const tests = [

    ];

    for (let f of tests) {
        await f(binding);
    }
}
