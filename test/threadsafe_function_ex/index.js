module.exports = (async () => {
    await require('./test/threadsafe')
    await require('./test/basic');
    await require('./test/example');
})();