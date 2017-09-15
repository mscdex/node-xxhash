var TransformStream = require('stream').Transform;
var inherits = require('util').inherits;

var addon = require('../build/Release/hash');
var XXHash32 = addon.XXHash32;
var XXHash64 = addon.XXHash64;

module.exports = XXHash32; // Backwards compatibility
module.exports.Stream = Stream;
module.exports.XXHash64 = XXHash64;
module.exports.hash64 = XXHash64.hash; // Convenience

function Stream(seed, bits, outEnc) {
  TransformStream.call(this);

  if (typeof bits === 'string') {
    outEnc = bits;
    bits = undefined;
  } else if (outEnc && typeof outEnc !== 'string' && !Buffer.isBuffer(outEnc)) {
    outEnc = 'buffer';
  }

  this._outEnc = outEnc;

  if (bits === 32 || bits === undefined)
    this._hash = new XXHash32(seed);
  else if (bits === 64)
    this._hash = new XXHash64(seed);
  else
    throw new Error('Bad bits argument');

  // Hack needed to support `readableObjectMode` in node v0.10 and to set
  // `highWaterMark` only for Readable side
  var rs = this._readableState;
  rs.objectMode = true;
  rs.highWaterMark = 2;
}
inherits(Stream, TransformStream);

Stream.prototype._transform = function(chunk, encoding, callback) {
  this._hash.update(chunk);
  callback();
};
Stream.prototype._flush = function(callback) {
  if (this._outEnc !== undefined)
    this.push(this._hash.digest(this._outEnc));
  else
    this.push(this._hash.digest());
  callback();
};
