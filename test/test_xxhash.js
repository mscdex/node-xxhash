var xxhash = require('../lib/xxhash')

exports.testXXHash = function (test) {
  var hash = xxhash.hash(new Buffer('hello'), 0xDEADBEEF)
  test.equal(hash, '2717969635')
  test.done()
}
