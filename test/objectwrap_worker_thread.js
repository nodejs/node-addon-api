'use strict';
const buildType = process.config.target_defaults.default_configuration;
const { Worker, isMainThread } = require('worker_threads');

if (isMainThread) {
    new Worker(__filename);
} else {
    const test = binding => {
        new binding.objectwrap.Test();
    };

    test(require(`./build/${buildType}/binding.node`));
    test(require(`./build/${buildType}/binding_noexcept.node`));
}
