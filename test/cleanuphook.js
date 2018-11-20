'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const child_process = require('child_process');
const bindingNode = `./build/${buildType}/binding.node`;
const bindingNoexceptNode = `./build/${buildType}/binding_noexcept.node`;

// we only check cleanup on 10.2 an higher were
function checkNodeVersion() {
  const nodeMajorVersion = process.versions.node.split('.')[0];
  const nodeMinorVersion = process.versions.node.split('.')[1];
  if (nodeMajorVersion >= 10 && nodeMinorVersion >= 2)
    return true;
  else
    return false;
}

function test(bindingType) {
  if (!checkNodeVersion())
    return;
  const { stdout } =
          child_process.spawnSync(process.execPath, [__filename, bindingType]);
  assert.strictEqual(stdout.toString().trim(), 'cleanup(42)');
}

if (process.argv[2] === bindingNode ||
    process.argv[2] === bindingNoexceptNode) {
  const binding = require(process.argv[2]);
  binding.cleanuphook.doCleanup();
}
else {
  test(bindingNode);
  test(bindingNoexceptNode);
}
