{
  'target_defaults': {
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
        'promise.cc',
        'typedarray.cc',
        'objectwrap.cc',
      ],
      'include_dirs': ["<!@(node -p \"require('../').include\")"],
      'dependencies': ["<!(node -p \"require('../').gyp\")"],
  },
  'targets': [
    {
      'target_name': 'binding',
      'defines': [ 'NAPI_CPP_EXCEPTIONS' ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      },
      'xcode_settings': {
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
      },
    },
    {
      'target_name': 'binding_noexcept',
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
      'cflags': [ '-fno-exceptions' ],
      'cflags_cc': [ '-fno-exceptions' ],
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 0 },
      },
      'xcode_settings': {
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
        'GCC_ENABLE_CPP_EXCEPTIONS': 'NO',
      },
    },
  ],
}
