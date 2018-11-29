# node-addon-api Changelog

## 2018-11-29 Version 1.6.2 (Current), @NickNaso

### Notable changes:

#### API

- Fixed selection logic for version 6.x.

### Commmits

* [[`07a0fc4e95`](https://github.com/nodejs/node-addon-api/commit/07a0fc4e95)] - **src**: fix selection logic for 6.x (Michael Dawson) [#402](https://github.com/nodejs/node-addon-api/pull/402)

## 2018-11-14 Version 1.6.1 (Current), @NickNaso

### Notable changes:

#### Documentation

- Updated links for examples to point to node-addon-examples repo.
- Fixed typos on some parts of documentation.

#### API

- Removed unused member on `Napi::CallbackScope`.
- Enabled `Napi::CallbackScope` only with N-API v3.

### Commmits

* [[`e7cd292a74`](https://github.com/nodejs/node-addon-api/commit/e7cd292a74)] - **src**: remove unused CallbackScope member (Gabriel Schulhof) [#391](https://github.com/nodejs/node-addon-api/pull/391)
* [[`d47399fe25`](https://github.com/nodejs/node-addon-api/commit/d47399fe25)] - **src**: guard CallbackScope with N-API v3 (Michael Dawson) [#395](https://github.com/nodejs/node-addon-api/pull/395)
* [[`29a0262ab9`](https://github.com/nodejs/node-addon-api/commit/29a0262ab9)] - **doc**: fix typo (Dongjin Na) [#385](https://github.com/nodejs/node-addon-api/pull/385)
* [[`b6dc15b88d`](https://github.com/nodejs/node-addon-api/commit/b6dc15b88d)] - **doc**: make links point to node-addon-examples repo (Nicola Del Gobbo) [#389](https://github.com/nodejs/node-addon-api/pull/389)

## 2018-11-02 Version 1.6.0 (Current), @NickNaso

### Notable changes:

#### Documentation

- Improved documentation about ABI stability.

#### API

- Add `Napi::CallbackScope` class that help to have the equivalent of the scope
associated with a callback in place when making certain N-API calls

#### TEST

- Added tests for `Napi::Array` class.
- Added tests for `Napi::ArrayBuffer` class.

### Commmits

* [[`8ce605c657`](https://github.com/nodejs/node-addon-api/commit/8ce605c657)] - **build**: avoid using package-lock.json (Jaeseok Yoon) [#359](https://github.com/nodejs/node-addon-api/pull/359)
* [[`fa3a6150b3`](https://github.com/nodejs/node-addon-api/commit/fa3a6150b3)] - **src**: use MakeCallback() -\> Call() in AsyncWorker (Jinho Bang) [#361](https://github.com/nodejs/node-addon-api/pull/361)
* [[`2342415463`](https://github.com/nodejs/node-addon-api/commit/2342415463)] - **test**: create test objects in the stack instead of the heap (Dongjin Na) [#371](https://github.com/nodejs/node-addon-api/pull/371)
* [[`67b7db0a6f`](https://github.com/nodejs/node-addon-api/commit/67b7db0a6f)] - **test**: write tests for Array class (Jaeseok Yoon) [#363](https://github.com/nodejs/node-addon-api/pull/363)
* [[`729f6dc4ee`](https://github.com/nodejs/node-addon-api/commit/729f6dc4ee)] - **test**: add arraybuffer tests (Dongjin Na) [#369](https://github.com/nodejs/node-addon-api/pull/369)
* [[`405f3e5b5b`](https://github.com/nodejs/node-addon-api/commit/405f3e5b5b)] - **src**: implement CallbackScope class (Jinho Bang) [#362](https://github.com/nodejs/node-addon-api/pull/362)
* [[`015d95312f`](https://github.com/nodejs/node-addon-api/commit/015d95312f)] - **doc**: fix Napi::Reference link (Gentilhomme) [#365](https://github.com/nodejs/node-addon-api/pull/365)
* [[`fd65078e3c`](https://github.com/nodejs/node-addon-api/commit/fd65078e3c)] - README.md: link to new ABI stability guide (Gabriel Schulhof) [#367](https://github.com/nodejs/node-addon-api/pull/367)
* [[`ffebf9ba9a`](https://github.com/nodejs/node-addon-api/commit/ffebf9ba9a)] - Updates for release 1.5.0 (NickNaso)

## 2018-10-03 Version 1.5.0 (Current), @NickNaso

### Notable changes:

#### Documentation

- Completed the documentation to cover all the API surface.
- Numerous fixes to make documentation more consistent in all of its parts.

#### API

- Add `Napi::AsyncContext` class to handle asynchronous operation.
- Add `Napi::BigInt` class to work with BigInt type.
- Add `Napi::VersionManagement` class to retrieve the versions of Node.js and N-API.
- Fix potential memory leaks.
- DataView feature is enabled by default
- Add descriptor for Symbols
- Add new methods on `Napi::FunctionReference`.
- Add the possibility to retrieve the environment on `Napi::Promise::Deferred`

#### TOOL

- Add tool to check if a native add-on is built using N-API

#### TEST

- Start to increase the test coverage
- Fix in the test suite to better handle the experimental features that are not
yet backported in the previous Node.js version.

### Commits

* [[`2009c019af`](https://github.com/nodejs/node-addon-api/commit/2009c019af)] - Merge pull request #292 from devsnek/feature/bigint (Gus Caplan)
* [[`e44aca985e`](https://github.com/nodejs/node-addon-api/commit/e44aca985e)] - add bigint class (Gus Caplan)
* [[`a3951ab973`](https://github.com/nodejs/node-addon-api/commit/a3951ab973)] - Add documentation for Env(). (Rolf Timmermans) [#318](https://github.com/nodejs/node-addon-api/pull/318)
* [[`a6f7a6ad51`](https://github.com/nodejs/node-addon-api/commit/a6f7a6ad51)] - Add Env() to Promise::Deferred. (Rolf Timmermans)
* [[`0097e96b92`](https://github.com/nodejs/node-addon-api/commit/0097e96b92)] - Fixed broken links for Symbol and String (NickNaso)
* [[`b0ecd38d76`](https://github.com/nodejs/node-addon-api/commit/b0ecd38d76)] - Fix Code of conduct link properly (#323) (Jake Yoon) [#323](https://github.com/nodejs/node-addon-api/pull/323)
* [[`223474900f`](https://github.com/nodejs/node-addon-api/commit/223474900f)] - **doc**: update Version management (Dongjin Na) [#360](https://github.com/nodejs/node-addon-api/pull/360)
* [[`4f76262a10`](https://github.com/nodejs/node-addon-api/commit/4f76262a10)] - **doc**: some fix on `Napi::Boolean` documentation (NickNaso) [#354](https://github.com/nodejs/node-addon-api/pull/354)
* [[`78374f72d2`](https://github.com/nodejs/node-addon-api/commit/78374f72d2)] - **doc**: number documentation (NickNaso) [#356](https://github.com/nodejs/node-addon-api/pull/356)
* [[`51ffe453f8`](https://github.com/nodejs/node-addon-api/commit/51ffe453f8)] - **doc**: doc cleanup (NickNaso) [#353](https://github.com/nodejs/node-addon-api/pull/353)
* [[`fc11c944b2`](https://github.com/nodejs/node-addon-api/commit/fc11c944b2)] - **doc**: major doc cleanup (NickNaso) [#335](https://github.com/nodejs/node-addon-api/pull/335)
* [[`100d0a7cb2`](https://github.com/nodejs/node-addon-api/commit/100d0a7cb2)] - **doc**: first pass on objectwrap documentation (NickNaso) [#321](https://github.com/nodejs/node-addon-api/pull/321)
* [[`c7d54180ff`](https://github.com/nodejs/node-addon-api/commit/c7d54180ff)] - **doc**: the Napi::ObjectWrap example does not compile (Arnaud Botella) [#339](https://github.com/nodejs/node-addon-api/pull/339)
* [[`7cdd78726a`](https://github.com/nodejs/node-addon-api/commit/7cdd78726a)] - **doc**: added cpp highlight for string.md (Jaeseok Yoon) [#329](https://github.com/nodejs/node-addon-api/pull/329)
* [[`8ed29f547c`](https://github.com/nodejs/node-addon-api/commit/8ed29f547c)] - **doc**: add blurb about ABI stability (Gabriel Schulhof) [#326](https://github.com/nodejs/node-addon-api/pull/326)
* [[`757eb1f5a3`](https://github.com/nodejs/node-addon-api/commit/757eb1f5a3)] - **doc**: add function and function reference doc (NickNaso) [#299](https://github.com/nodejs/node-addon-api/pull/299)
* [[`2885c18591`](https://github.com/nodejs/node-addon-api/commit/2885c18591)] - **doc**: Create changelog for release 1.4.0 (Nicola Del Gobbo)
* [[`917bd60baa`](https://github.com/nodejs/node-addon-api/commit/917bd60baa)] - **src**: remove TODOs by fixing memory leaks (Gabriel Schulhof) [#343](https://github.com/nodejs/node-addon-api/pull/343)
* [[`dfcb93945f`](https://github.com/nodejs/node-addon-api/commit/dfcb93945f)] - **src**: implement AsyncContext class (Jinho Bang) [#252](https://github.com/nodejs/node-addon-api/pull/252)
* [[`211ed38d0d`](https://github.com/nodejs/node-addon-api/commit/211ed38d0d)] - **src**: make 'nothing' target a static library (Gabriel Schulhof) [#348](https://github.com/nodejs/node-addon-api/pull/348)
* [[`97c4ab5cf2`](https://github.com/nodejs/node-addon-api/commit/97c4ab5cf2)] - **src**: add Call and MakeCallback that accept cargs (NickNaso) [#344](https://github.com/nodejs/node-addon-api/pull/344)
* [[`b6e2d92c09`](https://github.com/nodejs/node-addon-api/commit/b6e2d92c09)] - **src**: enable DataView feature by default (Jinho) [#331](https://github.com/nodejs/node-addon-api/pull/331)
* [[`0a00e7c97b`](https://github.com/nodejs/node-addon-api/commit/0a00e7c97b)] - **src**: implement missing descriptor defs for symbols (Philipp Renoth) [#280](https://github.com/nodejs/node-addon-api/pull/280)
* [[`38e01b7e3b`](https://github.com/nodejs/node-addon-api/commit/38e01b7e3b)] - **src**: first pass on adding version management apis (NickNaso) [#325](https://github.com/nodejs/node-addon-api/pull/325)
* [[`79ee8381d2`](https://github.com/nodejs/node-addon-api/commit/79ee8381d2)] - **src**: fix compile failure in test (Michael Dawson) [#345](https://github.com/nodejs/node-addon-api/pull/345)
* [[`4d92a6066f`](https://github.com/nodejs/node-addon-api/commit/4d92a6066f)] - **src**: Add ObjectReference test case (Anisha Rohra) [#212](https://github.com/nodejs/node-addon-api/pull/212)
* [[`779560f397`](https://github.com/nodejs/node-addon-api/commit/779560f397)] - **test**: add operator overloading tests in Number (Your Name) [#355](https://github.com/nodejs/node-addon-api/pull/355)
* [[`73fed84ceb`](https://github.com/nodejs/node-addon-api/commit/73fed84ceb)] - **test**: add ability to control experimental tests (Michael Dawson) [#350](https://github.com/nodejs/node-addon-api/pull/350)
* [[`14c69abd46`](https://github.com/nodejs/node-addon-api/commit/14c69abd46)] - **test**: write tests for Boolean class (Jaeseok Yoon) [#328](https://github.com/nodejs/node-addon-api/pull/328)
* [[`2ad47a83b1`](https://github.com/nodejs/node-addon-api/commit/2ad47a83b1)] - **test**: explicitly cast to uint32\_t in test (Gabriel Schulhof) [#341](https://github.com/nodejs/node-addon-api/pull/341)
* [[`622ffaea76`](https://github.com/nodejs/node-addon-api/commit/622ffaea76)] - **test**: Tighten up compiler warnings (Mikhail Cheshkov) [#315](https://github.com/nodejs/node-addon-api/pull/315)
* [[`fd3c37b0f2`](https://github.com/nodejs/node-addon-api/commit/fd3c37b0f2)] - **tools**: add tool to check for N-API modules (Gabriel Schulhof) [#346](https://github.com/nodejs/node-addon-api/pull/346)

## 2018-07-19 Version 1.4.0, @NickNaso

### Notable changes:

#### Documentation

- Numerous additions to the documentation, filling out coverage
  of API surface

#### API

- Add resource parameters to AsyncWorker constructor
- Add memory management feature

### Commits

* [[`7dc5ac8bc3`](https://github.com/nodejs/node-addon-api/commit/7dc5ac8bc3)] - **doc**: update metadata for release (Nicola Del Gobbo)
* [[`d68e86adb4`](https://github.com/nodejs/node-addon-api/commit/d68e86adb4)] - **doc**: Added documentation for PropertyDescriptor (Anisha Rohra) [#309](https://github.com/nodejs/node-addon-api/pull/309)
* [[`968a5f2000`](https://github.com/nodejs/node-addon-api/commit/968a5f2000)] - **doc**: Add documentation for ObjectReference.md (Anisha Rohra) [#307](https://github.com/nodejs/node-addon-api/pull/307)
* [[`908cdc314c`](https://github.com/nodejs/node-addon-api/commit/908cdc314c)] - **doc**: add `TypedArray` and `TypedArrayOf` (Kyle Farnung) [#305](https://github.com/nodejs/node-addon-api/pull/305)
* [[`2ff776ffe3`](https://github.com/nodejs/node-addon-api/commit/2ff776ffe3)] - backport node::Persistent (Gabriel Schulhof) [#300](https://github.com/nodejs/node-addon-api/pull/300)
* [[`98161970c9`](https://github.com/nodejs/node-addon-api/commit/98161970c9)] - Backport perf, crash and exception handling fixes (Gabriel Schulhof) [#295](https://github.com/nodejs/node-addon-api/pull/295)
* [[`dd1191e086`](https://github.com/nodejs/node-addon-api/commit/dd1191e086)] - **test**: fix asyncworker test so it runs on 6.x (Michael Dawson) [#298](https://github.com/nodejs/node-addon-api/pull/298)
* [[`11697fcecd`](https://github.com/nodejs/node-addon-api/commit/11697fcecd)] - **doc**: ArrayBuffer and Buffer documentation (Kyle Farnung) [#256](https://github.com/nodejs/node-addon-api/pull/256)
* [[`605aa2babf`](https://github.com/nodejs/node-addon-api/commit/605aa2babf)] - Add memory management feature (NickNaso) [#286](https://github.com/nodejs/node-addon-api/pull/286)
* [[`86be13a611`](https://github.com/nodejs/node-addon-api/commit/86be13a611)] - **doc**: Fix HandleScope docs (Ben Berman) [#287](https://github.com/nodejs/node-addon-api/pull/287)
* [[`90f92c4dc0`](https://github.com/nodejs/node-addon-api/commit/90f92c4dc0)] - **doc**: Update broken links in README.md (Hitesh Kanwathirtha) [#290](https://github.com/nodejs/node-addon-api/pull/290)
* [[`c2a620dc11`](https://github.com/nodejs/node-addon-api/commit/c2a620dc11)] - **doc**: Clarify positioning versus N-API (Michael Dawson) [#288](https://github.com/nodejs/node-addon-api/pull/288)
* [[`6cff890ee5`](https://github.com/nodejs/node-addon-api/commit/6cff890ee5)] - **doc**: Fix typo in docs (Ben Berman) [#284](https://github.com/nodejs/node-addon-api/pull/284)
* [[`7394bfd154`](https://github.com/nodejs/node-addon-api/commit/7394bfd154)] - **doc**: Fix typo in docs (Ben Berman) [#285](https://github.com/nodejs/node-addon-api/pull/285)
* [[`12b2cdeed3`](https://github.com/nodejs/node-addon-api/commit/12b2cdeed3)] - fix test files (Kyle Farnung) [#257](https://github.com/nodejs/node-addon-api/pull/257)
* [[`9ab6607242`](https://github.com/nodejs/node-addon-api/commit/9ab6607242)] - **doc**: Update Doc Version Number (joshgarde) [#277](https://github.com/nodejs/node-addon-api/pull/277)
* [[`e029a076c6`](https://github.com/nodejs/node-addon-api/commit/e029a076c6)] - **doc**: First pass at basic Node Addon API docs (Hitesh Kanwathirtha) [#268](https://github.com/nodejs/node-addon-api/pull/268)
* [[`74ff79717e`](https://github.com/nodejs/node-addon-api/commit/74ff79717e)] - **doc**: fix link to async\_worker.md (Michael Dawson)
* [[`5a63f45eda`](https://github.com/nodejs/node-addon-api/commit/5a63f45eda)] - **doc**: First step of error and async doc (NickNaso) [#272](https://github.com/nodejs/node-addon-api/pull/272)
* [[`9d38f61afb`](https://github.com/nodejs/node-addon-api/commit/9d38f61afb)] - **doc**: New Promise and Reference docs (Jim Schlight) [#243](https://github.com/nodejs/node-addon-api/pull/243)
* [[`43ff9fa836`](https://github.com/nodejs/node-addon-api/commit/43ff9fa836)] - **doc**: Updated Object documentation (Anisha Rohra) [#254](https://github.com/nodejs/node-addon-api/pull/254)
* [[`b197f7cc8b`](https://github.com/nodejs/node-addon-api/commit/b197f7cc8b)] - **doc**: minor typos (Nick Soggin) [#248](https://github.com/nodejs/node-addon-api/pull/248)
* [[`4b8918b352`](https://github.com/nodejs/node-addon-api/commit/4b8918b352)] - Add resource parameters to AsyncWorker constructor (Jinho Bang) [#253](https://github.com/nodejs/node-addon-api/pull/253)
* [[`1ecf7c19b6`](https://github.com/nodejs/node-addon-api/commit/1ecf7c19b6)] - **doc**: fix wrong link in readme (miloas) [#255](https://github.com/nodejs/node-addon-api/pull/255)
* [[`a750ed1932`](https://github.com/nodejs/node-addon-api/commit/a750ed1932)] - **release**: updates to metadata for next release (Michael Dawson)

## 2018-05-08 Version 1.3.0, @mhdawson

### Notable changes:

#### Documentation
- Added documentation for Scopes
- Added documentation for migration from NAN
- Update documentation to better explain the use of NODE_ADDON_API

#### API
- Implement data manipulation methods for dataview
- Use built-in N-API on Node.js >= 6.14.2
- Value
  - Added IsExternal()
  - IsObject() allow functions
- String
  - Fixed initialization of std::string to nullptr

#### Tests
- Fix test failures on linuxOne and AIX
- Added basic tests for Scopes
- Fix MSVC warning C4244 in tests

### Commits

* [386c2aeb74] - test: remove dep on later C++ feature (Michael Dawson) https://github.com/nodejs/node-addon-api/pull/267
* [10697734da] - Use built-in N-API on Node.js >= 6.14.2 (Gabriel Schulhof)
* [75086da273] - test: add basic tests and doc for scopes (Michael Dawson) https://github.com/nodejs/node-addon-api/pull/250
* [341dbd25d5] - doc: update blurb explaining NODE_ADDON_API (Gabriel Schulhof) https://github.com/nodejs/node-addon-api/pull/251
* [cf6c93e4ee] - don't try to escape null (Michael Dawson) https://github.com/nodejs/node-addon-api/pull/245
* [15e4b35fc2] - test: fix MSVC warning C4244 in tests (Kyle Farnung) https://github.com/nodejs/node-addon-api/pull/236
* [7f3ca03b8e] - Create a doc for migration (Sampson Gao) https://github.com/nodejs/node-addon-api/pull/118
* [0a2177debe] - Fix test failures on linuxOne and AIX (Jinho Bang) https://github.com/nodejs/node-addon-api/pull/232
* [d567f4b6b5] - Added Napi::Value::IsExternal() (Eric Bickle) https://github.com/nodejs/node-addon-api/pull/227
* [1b0f0e004a] - Update node-gyp.md (Michele Campus) https://github.com/nodejs/node-addon-api/pull/226
* [faf19c4f7a] - Fixed initialization of std::string to nullptr (Eric Bickle) https://github.com/nodejs/node-addon-api/pull/228
* [9c4d321b57] - Implement data manipulation methods for dataview (Jinho Bang) https://github.com/nodejs/node-addon-api/pull/218
* [5a39fdca6f] - n-api: throw RangeError napi_create_typedarray() (Jinho Bang) https://github.com/nodejs/node-addon-api/pull/216
* [1376377202] - Make IsObject() allow functions (Jinho Bang) https://github.com/nodejs/node-addon-api/pull/217
* [673b59d319] - src: Initial implementation of DataView class (Jinho Bang) https://github.com/nodejs/node-addon-api/pull/205
* [0a899bf1c5] - doc: update indication of latest version (Michael Dawson) https://github.com/nodejs/node-addon-api/pull/211
* [17c74e5a5e] - n-api: RangeError in napi_create_dataview() (Jinho Bang) https://github.com/nodejs/node-addon-api/pull/214
* [4058a29989] - n-api: fix memory leak in napi_async_destroy() (Jinho Bang) https://github.com/nodejs/node-addon-api/pull/213


