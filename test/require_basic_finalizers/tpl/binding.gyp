{
  'target_defaults': {
    'include_dirs': [
        "<!(node -p \"require('node-addon-api').include_dir\")"
    ],
    'variables': {
      'NAPI_VERSION%': "<!(node -p \"process.env.NAPI_VERSION || process.versions.napi\")",
      'require_basic_finalizers': "<!(node -p \"process.env['npm_config_require_basic_finalizers']\")",
    },
    'conditions': [
      ['NAPI_VERSION!=""', { 'defines': ['NAPI_VERSION=<@(NAPI_VERSION)'] } ],
      ['NAPI_VERSION==2147483647', { 'defines': ['NAPI_EXPERIMENTAL'] } ],
      ['require_basic_finalizers=="true"', {
        'defines': ['NODE_ADDON_API_REQUIRE_BASIC_FINALIZERS'],
      }],
      ['OS=="mac"', {
        'cflags+': ['-fvisibility=hidden'],
        'xcode_settings': {
          'OTHER_CFLAGS': ['-fvisibility=hidden']
        }
      }]
    ],
    'sources': [
      'addon.cc',
    ],
  },
  'targets': [
    {
      'target_name': 'addon',
      'defines': [
        'NAPI_CPP_EXCEPTIONS'
      ],
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
    }
  ]
}
