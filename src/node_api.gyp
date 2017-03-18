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
        'node_api_async.cc',
      ],
      'defines': [
         'EXTERNAL_NAPI',
      ],
    }
  ]
}
