{
  'targets': [
    {
      'target_name': 'binding',
      'sources': [
        'binding.cc',
        'error.cc',
        'function.cc',
        'name.cc',
      ],
      'include_dirs': ["<!(node -p \"require('../').include\")"],
      'dependencies': ["<!(node -p \"require('../').gyp\")"],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'xcode_settings': { 'GCC_ENABLE_CPP_EXCEPTIONS': 'YES' },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      },
    }
  ]
}
