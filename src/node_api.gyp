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
      ],
      'defines': [
         'EXTERNAL_NAPI',
      ],
    }
  ]
}
