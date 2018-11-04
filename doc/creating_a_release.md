# Creating a release

Only collaborators in npm for node-addon-api can create releases.
If you want to be able to do releases ask one of the existing
collaborators to add you.  If necessary you can ask the build
Working Group who manages the Node.js npm user to add you if
there are no other active collaborators.

These are the steps to follow to create a new release:

* Open an issue in the node-addon-api repo documenting
  the intent to create a new release.  Give people some
  time to comment or suggest PRs that should land first.

* Validate all tests pass by running npm test on master.

* Use https://ci.nodejs.org/view/x%20-%20Abi%20stable%20module%20API/job/node-test-node-addon-api/
  to validate tests pass for latest 9, 8, 6, 4 releases
  (note there are still some issues on SmartOS and
   Windows in the testing).

* Update the version in package.json appropriately.

* Update the README.md to show the new version as the latest.

* Add any new contributors to the "contributors" section in
  the package.json

* Do a clean checkout of node-addon-api.

* Login and then run `npm publish`.

* Create a release in Github (look at existing releases for an example).

* Validate that you can run `npm install node-addon-api` successfully
  and that the correct version is installed.

* Comment on the issue opened in the first step that the
  release has been created and close the issue.

* Tweet that the release has been created.
