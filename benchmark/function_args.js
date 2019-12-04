const path = require('path');
const Benchmark = require('benchmark');
const addonName = path.basename(__filename, '.js');

[ addonName, addonName + '_noexcept' ]
  .forEach((addonName) => {
    const rootAddon = require(`./build/Release/${addonName}`);
    const implems = Object.keys(rootAddon);
    const anObject = {};

    console.log(`${addonName}: `);

    console.log('no arguments:');
    implems.reduce((suite, implem) => {
      const fn = rootAddon[implem].noArgFunction;
      return suite.add(implem, () => fn());
    }, new Benchmark.Suite)
      .on('cycle', (event) => console.log(String(event.target)))
      .run();

    console.log('one argument:');
    implems.reduce((suite, implem) => {
      const fn = rootAddon[implem].oneArgFunction;
      return suite.add(implem, () => fn('x'));
    }, new Benchmark.Suite)
      .on('cycle', (event) => console.log(String(event.target)))
      .run();

    console.log('two arguments:');
    implems.reduce((suite, implem) => {
      const fn = rootAddon[implem].twoArgFunction;
      return suite.add(implem, () => fn('x', 12));
    }, new Benchmark.Suite)
      .on('cycle', (event) => console.log(String(event.target)))
      .run();

    console.log('three arguments:');
    implems.reduce((suite, implem) => {
      const fn = rootAddon[implem].threeArgFunction;
      return suite.add(implem, () => fn('x', 12, true));
    }, new Benchmark.Suite)
      .on('cycle', (event) => console.log(String(event.target)))
      .run();

    console.log('four arguments:');
    implems.reduce((suite, implem) => {
      const fn = rootAddon[implem].fourArgFunction;
      return suite.add(implem, () => fn('x', 12, true, anObject));
    }, new Benchmark.Suite)
      .on('cycle', (event) => console.log(String(event.target)))
      .run();
  });
