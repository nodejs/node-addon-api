// Makes sure that child processes are spawned appropriately.
const child_process = require('child_process');

const majorNodeVersion = process.versions.node.split('.')[0];
defaultArgs = [ '--expose-gc', '--no-concurrent-array-buffer-freeing' ];
if (majorNodeVersion >= 14) {
  defaultArgs.push('--no-concurrent-array-buffer-sweeping');
}

exports.spawnSync = function(command, args, options) {
  return child_process.spawnSync(command, defaultArgs.concat(args), options);
};

exports.spawn = function(command, args, options) {
  return child_process.spawn(command, defaultArgs.concat(args), options);
}
