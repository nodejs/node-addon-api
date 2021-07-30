const { spawn } = require("child_process");

module.exports.runChildProcess = async function (command, options) {
  const childProcess = spawn("node", [command], options);

  childProcess.stdout.on('data', data => {
    console.log(`${data}`);
  });
  childProcess.stderr.on('data', data => {
    console.log(`error: ${data}`);
  });

  return new Promise((resolve, reject) => {
    childProcess.on('error', (error) => {
      console.log(`error: ${error.message}`);
      reject(error);
    });
    childProcess.on('close', code => {
      console.log(`child process exited with code ${code}`);
      resolve();
    });
  });
}
