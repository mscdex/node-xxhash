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
      assert.strictEqual(hash.toString('hex'), '0c5badaa04926dcd');
    }
  },
  // Non-streaming hashing, custom encoding
  function(Hash, bits) {
    var hash = Hash.hash(input, SEED, 'hex');
    if (bits === 32) {
      assert.strictEqual(hash, 'a200ece3');
    } else {
      assert.strictEqual(hash, '0c5badaa04926dcd');
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
      assert.strictEqual(hash.toString('hex'), '0c5badaa04926dcd');
    }
  },
  // Streaming hashing, custom encoding
  function(Hash, bits) {
    var hash = new Hash(SEED);
    hash.update(input.slice(0, 2));
    hash.update(input.slice(2));
    hash = hash.digest('hex');
    if (bits === 32) {
      assert.strictEqual(hash, 'a200ece3');
    } else {
      assert.strictEqual(hash, '0c5badaa04926dcd');
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
      assert.strictEqual(hash.toString('hex'), '0c5badaa04926dcd');
    }
  },
  // Streaming (streams2+) hashing, custom encoding
  function(Hash, bits) {
    var hash = new XXHash.Stream(SEED, bits, 'hex');
    hash.write(input.slice(0, 2));
    hash.end(input.slice(2));
    hash = hash.read();
    if (bits === 32) {
      assert.strictEqual(hash, 'a200ece3');
    } else {
      assert.strictEqual(hash, '0c5badaa04926dcd');
    }
  },
].forEach(function(t) {
  t(XXHash, 32);
  t(XXHash64, 64);
});
