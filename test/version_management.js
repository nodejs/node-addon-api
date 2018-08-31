'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function parseVersion() {
    const expected = {};
    expected.napi = parseInt(process.versions.napi);
    expected.release = process.release.name;
    const nodeVersion = process.versions.node.split('.');
    expected.major = parseInt(nodeVersion[0]);
    expected.minor = parseInt(nodeVersion[1]);
    expected.patch = parseInt(nodeVersion[2]);
    return expected;
}

function test(binding) {

    const expected = parseVersion();

    const napiVersion = binding.version_management.getNapiVersion();
    assert.strictEqual(napiVersion, expected.napi);

    const nodeVersion = binding.version_management.getNodeVersion();
    assert.strictEqual(nodeVersion.major, expected.major);
    assert.strictEqual(nodeVersion.minor, expected.minor);
    assert.strictEqual(nodeVersion.patch, expected.patch);
    assert.strictEqual(nodeVersion.release, expected.release);

}
