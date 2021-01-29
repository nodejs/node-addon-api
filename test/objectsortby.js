'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');
const testUtil = require('./testUtil');

module.exports = test(require(`./build/${buildType}/binding.node`))
  .then(() => test(require(`./build/${buildType}/binding_noexcept.node`)));

function test(binding) {
  return testUtil.runGCTests([
    'sort object array by marks',
    ()=>{
        let objectarr=[{"name":"stu1","marks":10,"enrollNum":"en001"},{"name":"stu2","marks":20,"enrollNum":"en002"},{"name":"stu3","marks":6,"enrollNum":"en003"},{"name":"stu4","marks":15,"enrollNum":"en004"},{"name":"stu5","marks":11,"enrollNum":"en005"}];
        let expectedResult=[{"name":"stu3","marks":6,"enrollNum":"en003"},{"name":"stu1","marks":10,"enrollNum":"en001"},{"name":"stu5","marks":11,"enrollNum":"en005"},{"name":"stu4","marks":15,"enrollNum":"en004"},{"name":"stu2","marks":20,"enrollNum":"en002"}];
        let actualResult=binding.objectsortby.sortby(objectarr,"marks");
        for(var i=0;i<objectarr.length;i++){
            assert.strictEqual(actualResult[i].marks,expectedResult[i].marks);
            assert.strictEqual(actualResult[i].name,expectedResult[i].name);
            assert.strictEqual(actualResult[i].enrollNum,expectedResult[i].enrollNum);
        }
    },
  ]);
}