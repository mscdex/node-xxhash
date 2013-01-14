
Description
===========

An [xxhash](http://code.google.com/p/xxhash/) binding for [node.js](http://nodejs.org/).


Requirements
============

* [node.js](http://nodejs.org/) -- v0.6.0 or newer


Install
============

    npm install xxhash


Examples
========

* Hash a file in one step:

```javascript
var XXHash = require('xxhash'),
    fs = require('fs');

var file = fs.readFileSync('somefile'),
    result = XXHash.hash(file, 0xCAFEBABE);
```

* Hash a file in steps:

```javascript
var XXHash = require('xxhash'),
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


API
===

XXHash Static Methods
---------------------

* **hash**(< _Buffer_ >data, < _integer_ >seed) - _integer_ - Performs a single/one-time hash of `data` with the given `seed`. The resulting hash is returned.


XXHash Methods
--------------

* **(constructor)**(< _Integer_ >seed) - Create and return a new Hash instance that uses the given `seed`.

* **update**(< _Buffer_ >data) - _(void)_ - Update the hash using `data`. Note: the length of `data` must be a positive signed integer (e.g. 0 to 2,147,483,647 bytes).

* **digest**()  - _integer_ - Completes the hashing and returns the resulting integer hash. Note: hash object can not be used after digest() method been called.
