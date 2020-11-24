'use strict';
const { Worker, isMainThread, workerData } = require('worker_threads');

if (isMainThread) {
    const buildType = process.config.target_defaults.default_configuration;
    new Worker(__filename, { workerData: buildType });
} else {
    const test = binding => {
        new binding.objectwrap.Test();
    };

    const buildType = workerData;
    test(require(`./build/${buildType}/binding.node`));
    test(require(`./build/${buildType}/binding_noexcept.node`));
}
