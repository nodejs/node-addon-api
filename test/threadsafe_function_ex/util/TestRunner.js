// @ts-check
'use strict';
const assert = require('assert');
const { basename, extname } = require('path');
const buildType = process.config.target_defaults.default_configuration;

const pad = (what, targetLength = 20, padString = ' ', padLeft) => {
  const padder = (pad, str) => {
    if (typeof str === 'undefined')
      return pad;
    if (padLeft) {
      return (pad + str).slice(-pad.length);
    } else {
      return (str + pad).substring(0, pad.length);
    }
  };
  return padder(padString.repeat(targetLength), String(what));
}

/**
 * If `true`, always show results as interactive. See constructor for more
 * information.
*/
const SHOW_OUTPUT = false;

/**
 * Test runner helper class. Each static method's name corresponds to the
 * namespace the test as defined in the native addon. Each test specifics are
 * documented on the individual method. The async test handler runs
 * synchronously in the series of all tests so the test **MUST** wait on the
 * finalizer. Otherwise, the test runner will assume the test completed.
 */
class TestRunner {

  /**
   * @param {string} bindingKey The key to use when accessing the binding.
   * @param {string} filename Name of file that the current TestRunner instance
   * is being constructed. This determines how to log to console:
   * - When the test is running as the current module, output is shown on both
   *   start and stop of test in an 'interactive' styling.
   * - Otherwise, the output is more of a CI-like styling.
   */
  constructor(bindingKey, filename) {
    this.bindingKey = bindingKey;
    this.filename = filename;
    this.interactive = SHOW_OUTPUT || filename === require.main.filename;
    this.specName = `${this.bindingKey}/${basename(this.filename, extname(this.filename))}`;
  }

  async start() {
    try {
      this.log(`Running tests in .${this.filename.replace(process.cwd(), '')}:\n`);
      // Run tests in both except and noexcept
      await this.run(false);
      await this.run(true);
    } catch (ex) {
      console.error(`Test failed!`, ex);
      process.exit(1);
    }
  }

  /**
   * @param {boolean} isNoExcept If true, use the 'noexcept' binding.
   */
  async run(isNoExcept) {
    const binding = require(`../../build/${buildType}/binding${isNoExcept ? '_noexcept' : ''}.node`);
    const { bindingKey } = this;
    const spec = binding[bindingKey];
    const runner = this;

    // If we can't find the key in the binding, error.
    if (!spec) {
      throw new Error(`Could not find '${bindingKey}' in binding.`);
    }

    // A 'test' is defined as any function on the prototype of this object.
    for (const nsName of Object.getOwnPropertyNames(Object.getPrototypeOf(this))) {
      if (nsName !== 'constructor') {
        const ns = spec[nsName];

        // Interactive mode prints start and end messages
        if (this.interactive) {

          /** @typedef {[string, string | null | number, boolean, string, any]} State [label, time, isNoExcept, nsName, returnValue] */

          /** @type {State} */
          let state = [undefined, undefined, undefined, undefined, undefined]

          const stateLine = () => {
            const [label, time, isNoExcept, nsName, returnValue] = state;
            const except = () => pad(isNoExcept ? '[noexcept]' : '', 12);
            const timeStr = () => time == null ? '...' : `${time}${typeof time === 'number' ? 'ms' : ''}`;
            return `${pad(nsName, 10)} ${except()}|   ${pad(timeStr(), 8)}| ${pad(label, 15)}${returnValue === undefined ? '' : `(return: ${JSON.stringify(returnValue)})`}`;
          };

          /**
           * @param {string} label
           * @param {string | number} time
           * @param {boolean} isNoExcept
           * @param {string} nsName
           * @param {any} returnValue
           */
          const setState = (label, time, isNoExcept, nsName, returnValue) => {
            if (state[1] === null) {
              // Move to last line
              this.print(false, `\x1b[1A`);
            }
            state = [label, time, isNoExcept, nsName, returnValue];
            this.log(stateLine());
          };

          if (ns && typeof runner[nsName] === 'function') {
            setState('Running test', null, isNoExcept, nsName, undefined);
            const start = Date.now();
            const returnValue = await runner[nsName](ns);
            setState('Finished test', Date.now() - start, isNoExcept, nsName, returnValue);
          } else {
            setState('Skipping test', '-', isNoExcept, nsName, undefined);
          }
        } else if (ns) {
          console.log(`Running test '${this.specName}/${nsName}' ${isNoExcept ? '[noexcept]' : ''}`);
          await runner[nsName](ns);
        }
      }
    }
  }

  /**
   * Print to console only when using interactive mode.
   *
   * @param {boolean} newLine If true, end with a new line.
   * @param {any[]} what What to print
   */
  print(newLine, ...what) {
    if (this.interactive) {
      let target, method;
      target = newLine ? console : process.stdout;
      method = target === console ? 'log' : 'write';
      return target[method].apply(target, what);
    }
  }

  /**
   * Log to console only when using interactive mode.
   * @param {string[]} what
   */
  log(...what) {
    this.print(true, ...what);
  }
}

module.exports = {
  TestRunner
};
