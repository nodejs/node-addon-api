'use strict';

const assert = require('assert');

module.exports = require('../common').runTest(test);

// eslint-disable-next-line camelcase
function test ({ object_type_tag }) {
  const obj1 = object_type_tag.typedTaggedInstance(0);
  const obj2 = object_type_tag.typedTaggedInstance(1);
  const obj3 = object_type_tag.typedTaggedInstance(2);
  const obj4 = object_type_tag.typedTaggedInstance(3);

  // Verify that type tags are correctly accepted.
  assert.strictEqual(object_type_tag.checkTypeTag(0, obj1), true);
  assert.strictEqual(object_type_tag.checkTypeTag(1, obj2), true);
  assert.strictEqual(object_type_tag.checkTypeTag(2, obj3), true);
  assert.strictEqual(object_type_tag.checkTypeTag(3, obj4), true);

  // Verify that wrongly tagged objects are rejected.
  assert.strictEqual(object_type_tag.checkTypeTag(0, obj2), false);
  assert.strictEqual(object_type_tag.checkTypeTag(1, obj1), false);
  assert.strictEqual(object_type_tag.checkTypeTag(0, obj3), false);
  assert.strictEqual(object_type_tag.checkTypeTag(1, obj4), false);
  assert.strictEqual(object_type_tag.checkTypeTag(2, obj4), false);
  assert.strictEqual(object_type_tag.checkTypeTag(3, obj3), false);
  assert.strictEqual(object_type_tag.checkTypeTag(4, obj3), false);

  // Verify that untagged objects are rejected.
  assert.strictEqual(object_type_tag.checkTypeTag(0, {}), false);
  assert.strictEqual(object_type_tag.checkTypeTag(1, {}), false);
}
