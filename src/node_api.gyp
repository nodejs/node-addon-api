{
  'targets': [
    {
      'target_name': 'nothing'
    },
    {
      'target_name': 'node-api',
      'type': 'static_library',
      'sources': [
        'node_api.cc',
        'node_internals.cc',
      ],
      'defines': [
         'EXTERNAL_NAPI',
      ],
      'cflags_cc': ['-fvisibility=hidden'],
      'xcode_settings': {
        'OTHER_CFLAGS': ['-fvisibility=hidden']
      }
    }
  ]
}
