#!/usr/bin/env node

const spawnSync = require('child_process').spawnSync;
const path = require('path');

const filesToCheck = ['*.h', '*.cc'];
const FORMAT_START = process.env.FORMAT_START || 'main';
const pythonVersionCheck = [
  '-c',
  'import sys; raise SystemExit(sys.version_info[0] != 3)'
];

function findPython () {
  const candidates = [];

  if (process.env.PYTHON) {
    candidates.push({
      command: process.env.PYTHON,
      args: [],
      name: process.env.PYTHON
    });
  }

  if (process.platform === 'win32') {
    candidates.push({ command: 'py', args: ['-3'], name: 'py -3' });
  }

  candidates.push(
    { command: 'python3', args: [], name: 'python3' },
    { command: 'python', args: [], name: 'python' }
  );

  for (const candidate of candidates) {
    const result = spawnSync(
      candidate.command,
      [...candidate.args, ...pythonVersionCheck],
      { stdio: 'ignore' }
    );
    if (!result.error && result.status === 0) {
      return candidate;
    }
  }

  throw new Error([
    'Could not find a usable Python 3 executable.',
    `Tried: ${candidates.map(({ name }) => name).join(', ')}.`,
    'Set the PYTHON environment variable to the path of a Python 3 executable.'
  ].join('\n'));
}

function main (args) {
  let fix = false;
  while (args.length > 0) {
    switch (args[0]) {
      case '-f':
      case '--fix':
        fix = true;
        break;
      default:
    }
    args.shift();
  }

  const clangFormatPath = path.dirname(require.resolve('clang-format'));
  const binary = process.platform === 'win32'
    ? 'node_modules\\.bin\\clang-format.cmd'
    : 'node_modules/.bin/clang-format';
  const options = ['--binary=' + binary, '--style=file'];
  if (fix) {
    options.push(FORMAT_START);
  } else {
    options.push('--diff', FORMAT_START);
  }

  const gitClangFormatPath = path.join(clangFormatPath, 'bin/git-clang-format');
  let python;
  try {
    python = findPython();
  } catch (error) {
    console.error(error.message);
    return 2;
  }

  const result = spawnSync(
    python.command,
    [
      ...python.args,
      gitClangFormatPath,
      ...options,
      '--',
      ...filesToCheck
    ],
    { encoding: 'utf-8' }
  );

  if (result.error) {
    console.error('Error running git-clang-format:', result.error.message);
    return 2;
  }

  if (result.status !== 0 && result.status !== 1) {
    const message = (
      result.stderr ||
      result.stdout ||
      result.signal ||
      `exit code ${result.status}`
    ).trim();
    console.error(`Error running git-clang-format: ${message}`);
    return 2;
  }

  if (result.stderr) {
    process.stderr.write(result.stderr);
  }

  const clangFormatOutput = result.stdout.trim();
  // Bail fast if in fix mode.
  if (fix) {
    console.log(clangFormatOutput);
    return 0;
  }
  // Detect if there is any complains from clang-format
  if (
    clangFormatOutput !== '' &&
    clangFormatOutput !== 'no modified files to format' &&
    clangFormatOutput !== 'clang-format did not modify any files'
  ) {
    console.error(clangFormatOutput);
    const fixCmd = 'npm run lint:fix';
    console.error(`
      ERROR: please run "${fixCmd}" to format changes in your commit
        Note that when running the command locally, please keep your local
        main branch and working branch up to date with nodejs/node-addon-api
        to exclude un-related complains.
        Or you can run "env FORMAT_START=upstream/main ${fixCmd}".`);
    return 1;
  }
}

if (require.main === module) {
  process.exitCode = main(process.argv.slice(2));
}
