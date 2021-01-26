'use strict';

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`../build/${buildType}/binding.node`));
test(require(`../build/${buildType}/binding_noexcept.node`));


function test(binding) {
    const KEY_TYPE = {
        NAPI:  'KEY_AS_NAPI_VALUE',
        C_STR: 'KEY_AS_C_STRING',
        CPP_STR: 'KEY_AS_CPP_STRING',
        INT_32: 'KEY_AS_INT_32_NUM'
    };
    binding.globalObject.createMockTestObject();
    function assertGlobalObjectPropertyIs(key, attribute, keyType){
        let napiObjectAttr;
        switch(keyType)
        {
            case KEY_TYPE.NAPI:
            napiObjectAttr = binding.globalObject.getPropertyWithNapiValue(key);
            assert.deepStrictEqual(attribute, napiObjectAttr);
            break;

            case KEY_TYPE.C_STR:
            napiObjectAttr = binding.globalObject.getPropertyWithCString(key);
            assert.deepStrictEqual(attribute, napiObjectAttr);
            break;

            case KEY_TYPE.CPP_STR:
            napiObjectAttr = binding.globalObject.getPropertyWithCppString(key);
            assert.deepStrictEqual(attribute, napiObjectAttr);
            break;

            case KEY_TYPE.INT_32:
            napiObjectAttr = binding.globalObject.getPropertyWithInt32(key);
            assert.deepStrictEqual(attribute, napiObjectAttr);
            break;
        }
    }
 
    assertGlobalObjectPropertyIs('2',global['2'], KEY_TYPE.NAPI);
    assertGlobalObjectPropertyIs('c_str_key',global['c_str_key'],KEY_TYPE.C_STR);
    assertGlobalObjectPropertyIs('cpp_string_key',global['cpp_string_key'],KEY_TYPE.CPP_STR);
    assertGlobalObjectPropertyIs('circular',global['circular'],KEY_TYPE.CPP_STR);   
    assertGlobalObjectPropertyIs(15, global['15'], KEY_TYPE.INT_32); 
}
