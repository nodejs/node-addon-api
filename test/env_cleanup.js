'use strict';

const assert = require('assert');

if (process.argv[2] === 'runInChildProcess') {
    const binding_path = process.argv[3];
    const remove_hooks = process.argv[4] === "true";

    const binding = require(binding_path);
    binding.env_cleanup.addHooks(remove_hooks);
}
else {
    module.exports = require('./common').runTestWithBindingPath(test);
}

function test(bindingPath) {
    for (const remove_hooks of [false, true]) {
        const { status, output } = require('./napi_child').spawnSync(
            process.execPath,
            [
                __filename,
                'runInChildProcess',
                bindingPath,
                remove_hooks,
            ],
            { encoding: 'utf8' }
        );

        const stdout = output[1].trim();
        const lines = stdout.split(/[\r\n]+/).sort();

        assert(status === 0, `Process aborted with status ${status}`);

        if (remove_hooks) {
            assert.deepStrictEqual(lines, [''], 'Child process had console output when none expected')
        } else {
            assert.deepStrictEqual(lines, [
                "lambda cleanup()",
                "lambda cleanup(42)",
                "lambda cleanup(void)",
                "static cleanup()",
                "static cleanup(42)",
                "static cleanup(43)",
            ],  'Child process console output mismisatch')
        }
    }
}

