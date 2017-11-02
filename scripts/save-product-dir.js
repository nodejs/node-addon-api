const fs = require('fs');
const path = require('path');

fs.writeFileSync(
  path.resolve(path.join(__dirname, '..', 'product-dir.json')),
  JSON.stringify({productDir: process.argv[2].replace(/"/g, '')}));
