{
  'targets': [
    {
      'target_name': 'hash',
      'include_dirs': [ 'deps/xxhash' ],
      'sources': [
        'src/hash.cc',
        'deps/xxhash/xxhash.h',
        'deps/xxhash/xxhash.c',
      ],
      'cflags': [ '-O2' ],
    },
  ],
}