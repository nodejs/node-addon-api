const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

module.exports = require('./common').runTest(test);
function test(binding)
{
   console.log(binding.asyncWorkerUnitTest.runAsyncWorkerUnitTests());
}