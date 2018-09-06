'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));

function test(binding) {
	const testString = binding.basic_types_string.createUtf8String("κόσμε");
	assert.strictEqual(testString, "κόσμε");
}
