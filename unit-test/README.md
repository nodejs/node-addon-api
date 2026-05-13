
# Enable running tests with specific filter conditions:

The `--filter` option limits which test modules are executed by `node test`.
The default `pretest` step is still `node-gyp rebuild -C test`, so
`npm test --filter=...` still performs a full rebuild of the test addon
targets before the filtered tests run.

### Example:

  - perform the default test rebuild, then run only the `objectwrap`
    test module
```
    npm test --filter=objectwrap
```


# Wildcards are also possible:

### Example:

  - perform the default test rebuild, then run all test modules ending
    with `reference`
    (`function_reference`, `object_reference`, and `reference`)
```
    npm test --filter=*reference
```

# Multiple filter conditions are also allowed

### Example:

  - perform the default test rebuild, then run all tests under
    `threadsafe_function` and `typed_threadsafe_function`, and also the
    `objectwrap` test module
```
    npm test --filter='*function objectwrap'
```
