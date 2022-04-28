'use strict';

module.exports = require('../common').runTest(test);

function test (binding) {
  binding.typed_threadsafe_function_ptr.test({}, () => {});
}
