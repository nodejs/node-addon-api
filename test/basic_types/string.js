'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));

function test(binding) {
	const testUtf8String = binding.basic_types_string.createUtf8String("κόσμε");
	assert.strictEqual(testUtf8String, "κόσμε");

	const testUtf16String = binding.basic_types_string.createUtf16String("κόσμε");
	assert.strictEqual(testUtf16String, "κόσμε");
}
