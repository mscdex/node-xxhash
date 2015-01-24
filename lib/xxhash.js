var TransformStream = require('stream').Transform
                      || require('readable-stream').Transform,
    inherits = require('util').inherits;

var XXHash = module.exports = require('../build/Release/hash').XXHash;

XXHash.Stream = Stream;

function Stream(seed) {
  TransformStream.call(this);
  var rs = this._readableState;
  rs.objectMode = true;
  rs.highWaterMark = 2;
  this._hash = new XXHash(seed);
}
inherits(Stream, TransformStream);

Stream.prototype._transform = function(chunk, encoding, callback) {
  this._hash.update(chunk);
  callback();
};
Stream.prototype._flush = function(callback) {
  this.push(this._hash.digest());
  callback();
};
