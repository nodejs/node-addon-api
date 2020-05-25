// Run each test function in sequence,
// with an async delay and GC call between each.

function tick(x, cb) {
  function ontick() {
    if (--x === 0) {
      if (typeof cb === 'function') cb();
    } else {
      setImmediate(ontick);
    }
  }
  setImmediate(ontick);
};

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
        tick(10, runGCTests(tests, i + 1, title));
      });
    }
  }
}

module.exports = {
  runGCTests,
};
