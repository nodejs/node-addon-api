{
  'targets': [
    {
      'target_name': 'binding',
      'sources': [
        'arraybuffer.cc',
        'asyncworker.cc',
        'binding.cc',
        'buffer.cc',
        'error.cc',
        'external.cc',
        'function.cc',
        'name.cc',
        'object.cc',
      ],
      'include_dirs': ["<!(node -p \"require('../').include\")"],
      'dependencies': ["<!(node -p \"require('../').gyp\")"],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      },
    }
  ]
}
