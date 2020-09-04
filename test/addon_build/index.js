'use strict';

const { promisify } = require('util');
const  exec  = promisify(require('child_process').exec);
const { copy, remove } = require('fs-extra');
const path = require('path');
const assert = require('assert')

const addons = [
  'echo addon',
  'echo-addon'
]

async function before(addon) {
  await copy(path.join(__dirname, 'tpl'), path.join(__dirname, addon))
}

async function after(addon) {
  await remove(path.join(__dirname, addon));
}

async function test(addon) {
  await before(addon)
  const { stderr, stdout } = await exec('npm install', {
    cwd: path.join(__dirname, addon)
  })
  assert.strictEqual(stderr, '');
  assert.ok(stderr.length === 0);
  assert.ok(stdout.length > 0);
  const binding = require(`./${addon}`);
  assert.strictEqual(binding.except.echo('except'), 'except');
  assert.strictEqual(binding.except.echo(101), 101);
  assert.strictEqual(binding.noexcept.echo('noexcept'), 'noexcept');
  assert.strictEqual(binding.noexcept.echo(103), 103);
  await after(addon);
}


module.exports = (async function() {
  for (const addon of addons) {
    await test(addon)
  }

})()
