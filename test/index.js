'use strict';

let testModules = [
   'error',
   'function',
   'name',
];

testModules.forEach(name => {
   try {
      require('./' + name);
   }
   catch (e) {
      console.error(e);
   }
});
