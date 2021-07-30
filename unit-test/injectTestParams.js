const fs = require('fs');
const listOfTestModules = require('./listOfTestModules');

const buildDirs = listOfTestModules.dirs;
const buildFiles = listOfTestModules.files;

module.exports.filesToCompile = function () {
  const filterCondition = require('./matchModules').matchWildCards(process.env.filter || '');
  let files_to_compile = './generated/binding.cc test_helper.h';
  let conditions = filterCondition.split(' ').length ?  filterCondition.split(' ') : [filterCondition];
  let files = [];

  for (let matchCondition of conditions) {
    if (buildDirs[matchCondition.toLowerCase()]) {
      for (let file of buildDirs[matchCondition.toLowerCase()] ) {
        const config = buildFiles[file];
        const separator = config.dir.length ? '/' : ''
        files.push(config.dir + separator + file);
      }
    } else if (buildFiles[matchCondition.toLowerCase()]) {
      const config = buildFiles[matchCondition.toLowerCase()];
      const separator = config.dir.length ? '/' : ''
      files.push(config.dir + separator + matchCondition.toLowerCase());
    }
  }

  let addedFiles = '';
  files.forEach((file) => {
    addedFiles = `${addedFiles} ../test/${file}.cc`;
  });
  fs.writeFileSync(__dirname + '/generated/compilelist', `${files_to_compile} ${addedFiles}`);
  return `${files_to_compile} ${addedFiles}`;
};

module.exports.filesForBinding = function () {
  const filterCondition = require('./matchModules').matchWildCards(process.env.filter || '');
  fs.writeFileSync(__dirname + '/generated/bindingList', filterCondition);
  return filterCondition;
};


if (require.main === module) {
  const assert = require('assert');

  const setEnvAndCall = (fn, filterCondition) => { process.env.filter = filterCondition; return fn(); };

  assert.strictEqual(setEnvAndCall(exports.filesToCompile, 'typed*ex*'), './generated/binding.cc test_helper.h  ../test/typed_threadsafe_function/typed_threadsafe_function_existing_tsfn.cc');

  const expectedFilesToMatch = [
    './generated/binding.cc test_helper.h ',
    '../test/threadsafe_function/threadsafe_function.cc',
    '../test/threadsafe_function/threadsafe_function_ctx.cc',
    '../test/threadsafe_function/threadsafe_function_existing_tsfn.cc',
    '../test/threadsafe_function/threadsafe_function_ptr.cc',
    '../test/threadsafe_function/threadsafe_function_sum.cc',
    '../test/threadsafe_function/threadsafe_function_unref.cc',
    '../test/typed_threadsafe_function/typed_threadsafe_function.cc',
    '../test/typed_threadsafe_function/typed_threadsafe_function_ctx.cc',
    '../test/typed_threadsafe_function/typed_threadsafe_function_existing_tsfn.cc',
    '../test/typed_threadsafe_function/typed_threadsafe_function_ptr.cc',
    '../test/typed_threadsafe_function/typed_threadsafe_function_sum.cc',
    '../test/typed_threadsafe_function/typed_threadsafe_function_unref.cc'
  ]
  assert.strictEqual(setEnvAndCall(exports.filesToCompile, 'threadsafe_function typed_threadsafe_function'), expectedFilesToMatch.join(' '));

  assert.strictEqual(setEnvAndCall(exports.filesToCompile, 'objectwrap'), './generated/binding.cc test_helper.h  ../test/objectwrap.cc');

  console.log('ALL tests passed')
}
