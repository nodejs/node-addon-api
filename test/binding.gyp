{
  'variables': {
    'NAPI_VERSION%': "<!(node -p \"process.versions.napi\")",
    'NODE_MAJOR_VERSION%': "<!(node -p \"process.versions.node.match(/\\d+/)[0]\")",
    'disable_deprecated': "<!(node -p \"process.env['npm_config_disable_deprecated']\")"
  },
  'target_defaults': {
    'sources': [
        'arraybuffer.cc',
        'asynccontext.cc',
        'asyncprogressworker.cc',
        'asyncworker.cc',
        'asyncworker-persistent.cc',
        'basic_types/array.cc',
        'basic_types/boolean.cc',
        'basic_types/number.cc',
        'basic_types/value.cc',
        'bigint.cc',
        'date.cc',
        'binding.cc',
        'buffer.cc',
        'callbackscope.cc',
        'dataview/dataview.cc',
        'dataview/dataview_read_write.cc',
        'error.cc',
        'external.cc',
        'function.cc',
        'handlescope.cc',
        'memory_management.cc',
        'name.cc',
        'object/delete_property.cc',
        'object/finalizer.cc',
        'object/get_property.cc',
        'object/has_own_property.cc',
        'object/has_property.cc',
        'object/object.cc',
        'object/set_property.cc',
        'promise.cc',
        'threadsafe_function/threadsafe_function_ctx.cc',
        'threadsafe_function/threadsafe_function_existing_tsfn.cc',
        'threadsafe_function/threadsafe_function_ptr.cc',
        'threadsafe_function/threadsafe_function_sum.cc',
        'threadsafe_function/threadsafe_function_unref.cc',
        'threadsafe_function/threadsafe_function.cc',
        'typedarray.cc',
        'objectwrap.cc',
        'objectwrap_constructor_exception.cc',
        'objectreference.cc',
        'version_management.cc',
        'thunking_manual.cc',
      ],
      'conditions': [
        ['NAPI_VERSION!=""', { 'defines': ['NAPI_VERSION=<@(NAPI_VERSION)'] } ],
        ['disable_deprecated=="true"', {
          'defines': ['NODE_ADDON_API_DISABLE_DEPRECATED']
        }, {
          'sources': ['object/object_deprecated.cc']
        }],
        ['OS=="mac"', {
          'cflags+': ['-fvisibility=hidden'],
          'xcode_settings': {
            'OTHER_CFLAGS': ['-fvisibility=hidden']
          }
        }]
      ],
      'defines': ['NODE_MAJOR_VERSION=<@(NODE_MAJOR_VERSION)'],
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
