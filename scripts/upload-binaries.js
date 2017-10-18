const fs = require('fs');
const path = require('path');
const shelljs = require('shelljs');
const spawnSync = require( "child_process" ).spawnSync;
const pkg = require('../package.json');
const assignIn = require('lodash.assignin');

const repoPath = path.resolve(path.join(__dirname, '..', 'napi-binary-upload'));
const prebuildsPath = path.resolve(path.join(__dirname, '..', 'prebuilds'));

function run( command, args, options ) {
	options = options || {};
	const result = spawnSync( command, args, assignIn( {}, options, {
		stdio: [ process.stdin, process.stdout, process.stderr ],
		shell: true
	} ) );

	if ( result.status !== 0 ) {
		process.exit( result.status || 1 );
	}
}

run('git', [
  'clone',
  'napi-binary-upload:napi-binary-upload/upload',
  repoPath
]);

shelljs.cp.apply(shelljs, [
  '-f',
  fs.readdirSync(prebuildsPath).map((item) => {return path.join(prebuildsPath, item)}),
  repoPath
]);

run('git', ['add']
  .concat(fs
    .readdirSync(repoPath)
    .filter((item) => {return item !== '.git';})),
{cwd: repoPath});

run('git', [
  'commit',
  '-m',
  '"' + [pkg.version, process.platform, process.arch].join(' ') + '"'
], {cwd: repoPath});

run('git', ['push', 'origin', 'master'], {cwd: repoPath});

shelljs.rm('-rf', repoPath);
