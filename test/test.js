var XXHash = require('../lib/xxhash');
var XXHash64 = XXHash.XXHash64;

var assert = require('assert');

var SEED = 0xDEADBEEF;
var input = new Buffer('hello');

[
  // Non-streaming hashing
  function(Hash, bits) {
    var hash = Hash.hash(input, SEED);
    if (bits === 32) {
      assert.strictEqual(hash, 2717969635);
    } else {
      assert(Buffer.isBuffer(hash));
      assert.strictEqual(hash.toString('hex'), 'cd6d9204aaad5b0c');
    }
  },
  // Non-streaming hashing, custom encoding
  function(Hash, bits) {
    var hash = Hash.hash(input, SEED, 'hex');
    if (bits === 32) {
      assert.strictEqual(hash, 'e3ec00a2');
    } else {
      assert.strictEqual(hash, 'cd6d9204aaad5b0c');
    }
  },
  // Streaming hashing
  function(Hash, bits) {
    var hash = new Hash(SEED);
    hash.update(input.slice(0, 2));
    hash.update(input.slice(2));
    hash = hash.digest();
    if (bits === 32) {
      assert.strictEqual(hash, 2717969635);
    } else {
      assert(Buffer.isBuffer(hash));
      assert.strictEqual(hash.toString('hex'), 'cd6d9204aaad5b0c');
    }
  },
  // Streaming hashing, custom encoding
  function(Hash, bits) {
    var hash = new Hash(SEED);
    hash.update(input.slice(0, 2));
    hash.update(input.slice(2));
    hash = hash.digest('hex');
    if (bits === 32) {
      assert.strictEqual(hash, 'e3ec00a2');
    } else {
      assert.strictEqual(hash, 'cd6d9204aaad5b0c');
    }
  },
  // Streaming (streams2+) hashing
  function(Hash, bits) {
    var hash = new XXHash.Stream(SEED, bits);
    hash.write(input.slice(0, 2));
    hash.end(input.slice(2));
    hash = hash.read();
    if (bits === 32) {
      assert.strictEqual(hash, 2717969635);
    } else {
      assert(Buffer.isBuffer(hash));
      assert.strictEqual(hash.toString('hex'), 'cd6d9204aaad5b0c');
    }
  },
  // Streaming (streams2+) hashing, custom encoding
  function(Hash, bits) {
    var hash = new XXHash.Stream(SEED, bits, 'hex');
    hash.write(input.slice(0, 2));
    hash.end(input.slice(2));
    hash = hash.read();
    if (bits === 32) {
      assert.strictEqual(hash, 'e3ec00a2');
    } else {
      assert.strictEqual(hash, 'cd6d9204aaad5b0c');
    }
  },
].forEach(function(t) {
  t(XXHash, 32);
  t(XXHash64, 64);
});
