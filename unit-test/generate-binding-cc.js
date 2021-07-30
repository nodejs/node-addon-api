const fs = require('fs');
const path = require('path');
const listOfTestModules = require('./listOfTestModules');
const exceptions = require('./exceptions');
const generateFileContent = require('./binding-file-template').generateFileContent;

const buildDirs = listOfTestModules.dirs;
const buildFiles = listOfTestModules.files;

function generateBindingConfigurations() {
  const configs = [];

  const testFilesToBind = process.argv.slice(2);
  console.log('test modules to bind: ', testFilesToBind);

  testFilesToBind.forEach((file) => {
    const configName = file.split('.cc')[0];

    if (buildDirs[configName]) {
      for (let file of buildDirs[configName]) {
        if (exceptions.skipBinding.includes(file)) continue;
        configs.push(buildFiles[file]);
      }
    } else if (buildFiles[configName]) {
      configs.push(buildFiles[configName]);
    } else {
      console.log('not found', file, configName);
    }
  });

  return Promise.resolve(configs);
}

function writeToBindingFile(content) {
  const generatedFilePath = path.join(__dirname, 'generated', 'binding.cc' );
  fs.writeFileSync(generatedFilePath , "" );
  fs.writeFileSync(generatedFilePath, content, { flag: "a" } );
  console.log('generated binding file ',  generatedFilePath, new Date());
}

generateBindingConfigurations().then(generateFileContent).then(writeToBindingFile);
