## Creating prebuilds for a given platform

### Create an ssh alias
Add an alias called `napi-binary-upload` to your `~/.ssh/config`:
```
host napi-binary-upload
	Hostname github.com
	User git
  IdentityFile ~/.ssh/napi-binary-upload.id_rsa
```
The alias must be called `napi-binary-upload`. The identity file is optional,
but is useful when ensuring that access is restricted to the
`napi-binary-upload/upload` repository on Github.

### Build and upload binaries
Binaries are uploaded by amending the initial commit on
`napi-binary-upload/upload#master`. `scripts/binaries.js` runs from
`binding.gyp` as an epilogue to building the N-API implementation and performs
the upload.

```
git clean -x -d -f -f
npm install --ignore-scripts
npm run-script binaries
```

This will upload binaries for one platform/arch/node-version. You can use
[`nvm`](https://github.com/creationix/nvm/) to switch between Node.js versions
from a script.
