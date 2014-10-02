# xxhash-nan

An [xxhash](http://code.google.com/p/xxhash/) binding for [node.js](http://nodejs.org/). Forked from the [original](https://github.com/mscdex/node-xxhash) to add compatibility with future versions of node via NAN.

[![on npm](http://img.shields.io/npm/v/xxhash-nan.svg?style=flat)](https://www.npmjs.org/package/jthoober)  [![Tests](http://img.shields.io/travis/ceejbot/xxhash-nan.svg?style=flat)](http://travis-ci.org/ceejbot/xxhash-nan)    [![Dependencies](http://img.shields.io/david/ceejbot/xxhash-nan.svg?style=flat)](https://david-dm.org/ceejbot/xxhash-nan)

## Installation

You must be running node 0.8 or later. (Thanks to an npm 2.0 compatibility issue, travis tests fail on 0.8.)

  npm install xxhash

## Examples

* Hash a file in one step:

```javascript
var XXHash = require('xxhash-nan'),
    fs = require('fs');

var file = fs.readFileSync('somefile'),
    result = XXHash.hash(file, 0xCAFEBABE);
```

Hash a file incrementally:

```javascript
var XXHash = require('xxhash-nan'),
    fs = require('fs');

var hasher = new XXHash(0xCAFEBABE);

fs.createReadStream('somefile')
  .on('data', function(data) {
    hasher.update(data);
  })
  .on('end', function() {
    console.log('Hash value = ' + hasher.digest());
  });
```

## API

### XXHash module functions

* **hash**(< _Buffer_ >data, < _integer_ >seed) - _integer_ - Performs a single/one-time hash of `data` with the given `seed`. The resulting hash is returned.


### XXHash Methods

* **(constructor)**(< _Integer_ >seed) - Create and return a new Hash instance that uses the given `seed`.

* **update**(< _Buffer_ >data) - _(void)_ - Update the hash using `data`. Note: the length of `data` must be a positive signed integer (e.g. 0 to 2,147,483,647 bytes).

* **digest**()  - _integer_ - Completes the hashing and returns the resulting integer hash. Note: hash object can not be used after digest() method been called.
