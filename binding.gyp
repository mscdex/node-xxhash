{
  'targets': [
    {
      'target_name': 'hash',
      'defines': [
        '__USE_XOPEN2K8'
      ],
      'include_dirs': [
        'deps/xxhash',
        'src',
        "<!(node -e \"require('nan')\")",
      ],
      'sources': [
        'src/hash.cc',
        'src/hash_32.hpp',
        'src/hash_64.hpp',
        'src/common.hpp'
      ],
      'cflags': [ '-O2' ],
    },
  ],
}
