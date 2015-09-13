{
  'targets': [
    {
      'target_name': 'hash',
      'include_dirs': [
        'deps/xxhash',
        'src',
        "<!(node -e \"require('nan')\")",
      ],
      'sources': [
        'src/hash.cc',
        'src/hash_32.hpp',
        'src/hash_64.hpp',
        'src/common.hpp',
        'deps/xxhash/xxhash.h',
        'deps/xxhash/xxhash.c',
      ],
      'cflags': [ '-O2' ],
    },
  ],
}