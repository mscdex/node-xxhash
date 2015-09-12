var xxhash = require('../lib/xxhash');

var assert = require('assert');

var hash;

hash = xxhash.hash(new Buffer('hello'), 0xDEADBEEF);
assert.strictEqual(hash, 2717969635);

hash = new xxhash(0xDEADBEEF);
hash.update(new Buffer('hello'));
assert.strictEqual(hash.digest(), 2717969635);
