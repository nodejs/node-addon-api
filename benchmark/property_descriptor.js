const path = require('path');
const Benchmark = require('benchmark');
const addonName = path.basename(__filename, '.js');

[ addonName, addonName + '_noexcept' ]
  .forEach((addonName) => {
    const rootAddon = require(`./build/Release/${addonName}`);
    const getters = new Benchmark.Suite;
    const setters = new Benchmark.Suite;

    console.log(`${addonName}: `);

    Object.keys(rootAddon).forEach((key) => {
      getters.add(`${key} getter`, () => {
        const x = rootAddon[key];
      });
      setters.add(`${key} setter`, () => {
        rootAddon[key] = 5;
      })
    });

    getters
      .on('cycle', (event) => console.log(String(event.target)))
      .run();

    setters
      .on('cycle', (event) => console.log(String(event.target)))
      .run();
  });
