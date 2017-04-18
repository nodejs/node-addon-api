'use strict';

let testModules = [
   'error',
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
