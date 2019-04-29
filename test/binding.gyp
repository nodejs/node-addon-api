{
  'target_defaults': {
    'includes': ['../common.gypi'],
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
        'run_script.cc',
        'threadsafe_function/threadsafe_function_ctx.cc',
        'threadsafe_function/threadsafe_function_existing_tsfn.cc',
        'threadsafe_function/threadsafe_function_ptr.cc',
        'threadsafe_function/threadsafe_function_sum.cc',
        'threadsafe_function/threadsafe_function_unref.cc',
        'threadsafe_function/threadsafe_function.cc',
        'typedarray.cc',
        'objectwrap.cc',
        'objectwrap_constructor_exception.cc',
        'objectwrap-removewrap.cc',
        'objectreference.cc',
        'version_management.cc',
        'thunking_manual.cc',
      ],
      'conditions': [
        ['disable_deprecated!="true"', {
          'sources': ['object/object_deprecated.cc']
        }]
      ],
  },
  'targets': [
    {
      'target_name': 'binding',
      'includes': ['../except.gypi']
    },
    {
      'target_name': 'binding_noexcept',
      'includes': ['../noexcept.gypi']
    },
  ],
}
