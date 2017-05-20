// Run each test function in sequence,
// with an async delay and GC call between each.
function runGCTests(tests, i, title) {
  if (!i) {
     i = 0;
  }

  if (tests[i]) {
    if (typeof tests[i] === 'string') {
      title = tests[i];
      runGCTests(tests, i + 1, title);
    } else {
      try {
        tests[i]();
      } catch (e) {
        console.error('Test failed: ' + title);
        throw e;
      }
      setImmediate(() => {
        global.gc();
        runGCTests(tests, i + 1, title);
      });
    }
  }
}

module.exports = {
   runGCTests,
};
