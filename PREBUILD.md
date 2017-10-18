## Creating a prebuild for a certain platform/arch

1. Set up an ssh host which must be named `napi-binary-upload` for committing to
the repo napi-binary-upload/upload. This is done in case the ssh key for
committing to the napi-binary-upload/upload repo must be different than the one
for committing to other github repositories.
    ```
    host napi-binary-upload
      Hostname github.com
      User git
      IdentityFile ~/.ssh/napi-binary-upload-ssh-key
    ```

    The ssh secret key file is optional and can be at any path and it may have
    any name.

2. `git clean -x -d -f -f`
3. `npm install`
4. `npm run-script binaries`
