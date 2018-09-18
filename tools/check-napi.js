// Descend into a directory structure and, for each file matching *.node, output
// based on the imports found in the file whether it's an N-API module or not.

const fs = require('fs');
const path = require('path');
const child_process = require('child_process');

// Read the output of the command, break it into lines, and use the reducer to
// decide whether the file is an N-API module or not.
function checkFile(file, command, arguments, reducer) {
  const child = child_process.spawn(command, arguments, {
    stdio: ['inherit', 'pipe', 'inherit']
  });
  let leftover = '';
  let isNapi = undefined;
  child.stdout.on('data', (chunk) => {
    if (isNapi === undefined) {
      chunk = leftover + chunk.toString();
      const haveLeftover = !!chunk.match(/[\r\n]+$/);
      chunk = chunk.split(/[\r\n]+/);
      leftover = chunk.pop();
      isNapi = chunk.reduce(reducer, isNapi);
    }
  });
  child.on('exit', (code, signal) => {
    if ((code === null && signal !== null) || (code !== 0)) {
      console.log(
        command + ' exited with code: '  + code + ' and signal: ' + signal);
    } else {
      console.log(
          '\033[' + (isNapi ? '42' : '41') + 'm' +
          (isNapi ? '    N-API' : 'Not N-API') +
          '\033[0m: ' + file);
    }
  });
}

// Use nm -a to list symbols.
function checkFileUNIX(file) {
  checkFile(file, 'nm', ['-a', file], (soFar, line) => {
      if (soFar === undefined) {
        line = line.match(/([0-9a-f]*)? ([a-zA-Z]) (.*$)/);
        line.shift();
        if (line[1] === 'U') {
          if (line[2].match(/^napi/)) {
            soFar = true;
          }
        }
      }
      return soFar;
  });
}

// Use dumpbin /imports to list symbols.
function checkFileWin32(file) {
  checkFile(file, 'dumpbin', ['/imports', file], (soFar, line) => {
      if (soFar === undefined) {
        line = line.match(/([0-9a-f]*)? +([a-zA-Z0-9]) (.*$)/);
        if (line && line[line.length - 1].match(/^napi/)) {
          soFar = true;
        }
      }
      return soFar;
  });
}

// Descend into a directory structure and pass each file ending in '.node' to
// one of the above checks, depending on the OS.
function recurse(top) {
  fs.readdir(top, (error, items) => {
    items.forEach((item) => {
      item = path.join(top, item);
      fs.stat(item, ((item) => (error, stats) => {
        if (!error) {
          if (stats.isDirectory()) {
            recurse(item);
          } else if (item.match(/[.]node$/) &&
              // Explicitly ignore files called 'nothing.node' because they are
              // artefacts of node-addon-api having identified a version of
              // Node.js that ships with a correct implementation of N-API.
              path.basename(item) !== 'nothing.node') {
            process.platform === 'win32' ?
                checkFileWin32(item) :
                checkFileUNIX(item);
          }
        } else {
          throw ("error about " + item + ": " + error);
        }
      })(item));
    });
  });
}

// Start with the directory given on the command line or the current directory
// if nothing was given.
recurse(process.argv.length > 3 ? process.argv[2] : '.');
