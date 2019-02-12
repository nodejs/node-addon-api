{
  'variables': {
    'NAPI_VERSION%': "",
    'disable_deprecated': "<!(node -p \"process.env['npm_config_disable_deprecated']\")"
  },
  'target_defaults': {
    'sources': [
        'asyncworker-persistent.cc',
        'binding.cc',
      ],
      'conditions': [
        ['NAPI_VERSION!=""', { 'defines': ['NAPI_VERSION=<@(NAPI_VERSION)'] } ],
        ['disable_deprecated=="true"', {
          'defines': ['NODE_ADDON_API_DISABLE_DEPRECATED']
        }, {
          'sources': ['object/object_deprecated.cc']
        }]
      ],
      'include_dirs': ["<!@(node -p \"require('../').include\")"],
      'dependencies': ["<!(node -p \"require('../').gyp\")"],
      'cflags': [ '-Werror', '-Wall', '-Wextra', '-Wpedantic', '-Wunused-parameter' ],
      'cflags_cc': [ '-Werror', '-Wall', '-Wextra', '-Wpedantic', '-Wunused-parameter' ]
  },
  'targets': [
    {
      'target_name': 'binding',
      'defines': [ 'NAPI_CPP_EXCEPTIONS' ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'msvs_settings': {
        'VCCLCompilerTool': {
          'ExceptionHandling': 1,
          'EnablePREfast': 'true',
        },
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
        'VCCLCompilerTool': {
          'ExceptionHandling': 0,
          'EnablePREfast': 'true',
        },
      },
      'xcode_settings': {
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
        'GCC_ENABLE_CPP_EXCEPTIONS': 'NO',
      },
    },
  ],
}
