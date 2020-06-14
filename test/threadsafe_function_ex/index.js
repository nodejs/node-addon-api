const tests = [
    // 'threadsafe', 
    'basic', 
    'example'
];

// Threadsafe tests must run synchronously. If two threaded-tests are running
// and one fails, Node may exit while `std::thread`s are running.
module.exports = (async () => {
    for (const test of tests) {
        await require(`./test/${test}`);
    }
})();
