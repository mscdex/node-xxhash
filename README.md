
Description
===========

An [xxhash](https://github.com/Cyan4973/xxHash) binding for [node.js](http://nodejs.org/).

[![Build Status](https://travis-ci.org/mscdex/node-xxhash.svg?branch=master)](https://travis-ci.org/mscdex/node-xxhash)
[![Build status](https://ci.appveyor.com/api/projects/status/y2f8pisjshf4g181)](https://ci.appveyor.com/project/mscdex/node-xxhash)


Requirements
============

* [node.js](http://nodejs.org/) -- v4.0.0 or newer


Install
============

    npm install xxhash


Examples
========

* Hash a file in one step:

```javascript
var XXHash = require('xxhash');
var fs = require('fs');

var file = fs.readFileSync('somefile');
var result = XXHash.hash(file, 0xCAFEBABE);
```

* Hash a file in multiple steps:

```javascript
var XXHash = require('xxhash');
var fs = require('fs');

var hasher = new XXHash(0xCAFEBABE);

fs.createReadStream('somefile')
  .on('data', function(data) {
    hasher.update(data);
  })
  .on('end', function() {
    console.log('Hash value = ' + hasher.digest());
  });
```

* Hash a file with a hash stream:

```javascript
var HashStream = require('xxhash').Stream;
var fs = require('fs');

var hasher = new HashStream(0xCAFEBABE);

fs.createReadStream('somefile')
  .pipe(hasher)
  .on('finish', function() {
    console.log('Hash value = ' + hasher.read());
  });
```


API
===

XXHash Static Methods
---------------------

* **hash**(< _Buffer_ >data, < _mixed_ >seed[, < _mixed_ >encbuf]) - _mixed_ - Performs a single/one-time 32-bit hash of `data` with the given `seed`. `seed` can be an unsigned integer or a Buffer containing (1 <= n <= 4) bytes to use for the seed. The resulting hash is returned. The format of the hash depends on the value of `encbuf`. If `encbuf` is a string and one of: `buffer`, `hex`, `base64`, or `binary`, then the hash value will be encoded in the appropriate format. If `encbuf` is a _Buffer_ of at least 4 bytes, then the hash value will be written to `encbuf` and `encbuf` will be returned. Otherwise, if `encbuf` is not supplied, then the hash will be an unsigned integer.

* **hash64**(< _Buffer_ >data, < _mixed_ >seed[, < _mixed_ >encbuf]) - _mixed_ - Performs a single/one-time 64-bit hash of `data` with the given `seed`. `seed` can be an unsigned integer or a Buffer containing (1 <= n <= 8) bytes to use for the seed. The resulting hash is returned. The format of the hash depends on the value of `encbuf`. If `encbuf` is a string and one of: `buffer`, `hex`, `base64`, or `binary`, then the hash value will be encoded in the appropriate format. If `encbuf` is a _Buffer_ of at least 8 bytes, then the hash value will be written to `encbuf` and `encbuf` will be returned. The default value for `encbuf` is `'buffer'`.


XXHash Static Properties
------------------------

* **Stream(< _mixed_ >seed[, < _integer_ >bits][, < _mixed_ >encbuf])** - _DuplexStream_ - A stream constructor that takes in the `seed` to use. Write data to the stream and when the stream ends, a `bits`-bit (32 or 64) hash value (format determined by `encbuf`) is available on the readable side. The values for `seed` and `encbuf` are described above in `hash()`.

* **XXHash64(< _mixed_ >seed)** - This is the 64-bit Hash constructor. It is only needed if you want to use the old streaming interface (`update()`/`digest()`) instead of the streams2 interface described above.


XXHash Methods
--------------

* **(constructor)**(< _mixed_ >seed) - Creates and returns a new 32-bit Hash instance with the given `seed`. The values for `seed` are described above in `hash()`.

* **update**(< _Buffer_ >data) - _(void)_ - Update the hash using `data`.

* **digest**([< _mixed_ >encbuf])  - _mixed_ - The values for `encbuf` and the resulting hash value format is described in `hash()`.
