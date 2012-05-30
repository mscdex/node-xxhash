
Description
===========

An [xxhash](http://code.google.com/p/xxhash/) binding for [node.js](http://nodejs.org/).


Requirements
============

* [node.js](http://nodejs.org/) -- v0.6.0 or newer


License
=======

BSD 2-Clause


Install
============

npm install xxhash


Examples
========

* Hash a file using the fast method:
```javascript
  var xxhash = require('xxhash'),
        fs = require('fs');

  var file = fs.readFileSync('somefile'),
        result = xxhash.fast(file, 0xCAFEBABE);
```

* Hash a file using the strong method:
```javascript
  var xxhash = require('xxhash'),
        fs = require('fs');

  var file = fs.readFileSync('somefile'),
        result = xxhash.strong(file, 0xCAFEBABE);
```


API
===

Static Methods
--------------

* **fast**(<_Buffer_>data, <_Integer_>seed) - <_Integer_> - Hashes _data_ with the given _seed_ using the fast version of xxhash. The resulting hash is returned.

* **strong**(<_Buffer_>data, <_Integer_>seed) - <_Integer_> - Hashes _data_ with the given _seed_ using the strong version of xxhash. The resulting hash is returned.
