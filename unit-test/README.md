
# Enable running tests with specific filter conditions:

### Example:

  - compile ad run only tests on objectwrap.cc and objectwrap.js
```
    npm run unit --filter=objectwrap
```


# Wildcards are also possible:

### Example:

  - compile and run all tests files ending with reference -> function_reference.cc object_reference.cc reference.cc
```
    npm run unit --filter=*reference
``` 

# Multiple filter conditions are also allowed

### Example:

  - compile and run all tests under folders threadsafe_function and typed_threadsafe_function and also the objectwrap.cc file
```
    npm run unit --filter='*function objectwrap'
```
