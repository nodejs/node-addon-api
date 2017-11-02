var fs = require('fs');
var path = require('path');
var spawnSync = require('child_process').spawnSync;
var shelljs = require('shelljs');
var version = require('../package.json').version;
var productDir = require('../product-dir.json').productDir;
var config = require('./helpers/config');
var tar = require('tar');

var libFile;
var repoPath = path.join(path.resolve(path.join(__dirname, '..')), 'upload');
var implemSource = config.sourceName + '.' + config.productExtension;
var implemDestination = config.makeFileName(process.versions.modules);
var tarballFiles = [];

function makeTarball(name, tarballFiles) {
	return tar.c({
		gzip: true,
		file: path.join(repoPath, name),
		sync: true,
		cwd: repoPath
	}, tarballFiles);
}

function git(args, options) {
	var result = spawnSync( 'git', args, Object.assign({}, options || {}, {
    stdio: [ process.stdin, process.stdout, process.stderr ],
		shell: true
  }));

  if (result.signal || result.status !== 0) {
		console.error('Error running git: signal: ' + result.signal +
			', status: ' + result.status);
		process.exit(1);
	}
}

// Clone the repo.
git(['clone', 'napi-binary-upload:napi-binary-upload/upload', repoPath]);

// Copy the implementation and collect the file names to add to the tarball.
shelljs.cp(
  path.join(productDir, implemSource),
  path.join(repoPath, implemDestination));
tarballFiles.push(implemDestination);
if (process.platform === 'win32') {
	libFile = config.makeFileName(process.versions.modules, 'lib');
  shelljs.cp(
    path.join(productDir,
      path.basename(implemSource, '.' + config.productExtension) + '.lib'),
    path.join(repoPath, libFile));
  tarballFiles.push(libFile);
}

// We make the 6.x implementation the default and mark its files for addition to
// the tarball as well.
if (process.versions.modules === '48') {
	shelljs.cp(path.join(productDir, implemSource), repoPath);
	tarballFiles.push(implemSource);
	if (process.platform === 'win32') {
		libFile =
			path.basename(implemSource, '.' + config.productExtension) + '.lib';
		shelljs.cp(path.join(productDir, libFile), repoPath);
		tarballFiles.push(libFile);
	}
}

makeTarball(config.makeFileName(process.versions.modules, 'tgz'), tarballFiles);

// Mark all files for commit.
git(['add'].concat(
  fs.readdirSync(repoPath).filter((item) => (item.match(/[.]tgz$/)))
), {cwd: repoPath});

// Commit the files.
git(['commit', '--amend', '-m',
  '"' + [version, process.platform, process.arch].join(' ') + '"'
],{cwd: repoPath});

// Push to origin.
git(['push', '-f', 'origin', 'master'], {cwd: repoPath});

// Remove the repo.
shelljs.rm("-rf", repoPath);
