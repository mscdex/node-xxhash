{
  'targets': [
    {
      'target_name': 'hash',
      'include_dirs': [ 'deps/xxhash' ],
      'sources': [
        'hash.cc',
        'deps/xxhash/xxhash.h',
        'deps/xxhash/xxhash.cc',
      ],
      'cflags': [ '-O3' ],
    },
  ],
}