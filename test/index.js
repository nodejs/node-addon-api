'use strict';

if (typeof global.gc !== 'function') {
   throw new Error('Tests require --expose-gc flag.')
}

let testModules = [
   'arraybuffer',
   'buffer',
   'error',
   'external',
   'function',
];

testModules.forEach(name => {
   try {
      require('./' + name);
   }
   catch (e) {
      console.error(e);
   }
});
