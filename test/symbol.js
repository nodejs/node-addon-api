'use strict';

const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

module.exports = require('./common').runTest(test);


async function test(binding)
{

    const wellKnownSymbolFunctions = ['asyncIterator','hasInstance','isConcatSpreadable', 'iterator','match','matchAll','replace','search','split','species','toPrimitive','toStringTag','unscopables'];

    function assertCanCreateSymbol(symbol)
    {
      assert(binding.symbol.createNewSymbolWithCppStr(symbol) !== null);
      assert(binding.symbol.createNewSymbolWithCStr(symbol) !== null);
      assert(binding.symbol.createNewSymbolWithNapi(symbol) !== null);
    }

    function assertSymbolAreUnique(symbol)
    {
        const symbolOne = binding.symbol.createNewSymbolWithCppStr(symbol);
        const symbolTwo = binding.symbol.createNewSymbolWithCppStr(symbol);

        assert(symbolOne !== symbolTwo);
    }

    function assertSymbolIsWellknown(symbol)
    {
      const symbOne = binding.symbol.getWellKnownSymbol(symbol);
      const symbTwo = binding.symbol.getWellKnownSymbol(symbol);
      assert(symbOne && symbTwo);
      assert(symbOne === symbTwo);
    }

    function assertSymbolIsNotWellknown(symbol)
    {
      const symbolTest = binding.symbol.getWellKnownSymbol(symbol);
      assert(symbolTest === undefined);
    }

    function assertCanCreateOrFetchGlobalSymbols(symbol, fetchFunction)
    {
      const symbOne = fetchFunction(symbol);
      const symbTwo = fetchFunction(symbol);
      assert(symbOne && symbTwo);
      assert(symbOne === symbTwo);
    }

    assertCanCreateSymbol("testing");
    assertSymbolAreUnique("symbol");
    assertSymbolIsNotWellknown("testing");

     for(const wellknownProperty of wellKnownSymbolFunctions)
     {
       assertSymbolIsWellknown(wellknownProperty);
     }

     assertCanCreateOrFetchGlobalSymbols("data", binding.symbol.getSymbolFromGlobalRegistry);
     assertCanCreateOrFetchGlobalSymbols("CppKey", binding.symbol.getSymbolFromGlobalRegistryWithCppKey);
     assertCanCreateOrFetchGlobalSymbols("CKey", binding.symbol.getSymbolFromGlobalRegistryWithCKey);

    assert(binding.symbol.createNewSymbolWithNoArgs() === undefined);

    assert(binding.symbol.testNullSymbolCanBeCreated() === binding.symbol.testNullSymbolCanBeCreated());
    assert(binding.symbol.testUndefinedSymbolCanBeCreated() === binding.symbol.testUndefinedSymbolCanBeCreated());
    assert(binding.symbol.testUndefinedSymbolCanBeCreated() !== binding.symbol.testNullSymbolCanBeCreated());
}
