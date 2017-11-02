{
  'variables': {
    'binaries': "<!(node -p \"process.env.npm_lifecycle_event === 'binaries'\")"
  },
  'targets': [
    {
      'target_name': 'save-product-dir',
      'type': 'none',
      'actions': [
        {
          'action_name': 'save-product-dir',
          'inputs': [''],
          'outputs': ['product-dir.json'],
          'action': [
            'node',
            'scripts/save-product-dir.js',
            '<(PRODUCT_DIR)'
          ]
        }
      ]
    }
  ],
  'conditions': [
    [ "binaries=='true'", {
      'targets': [
        {
          'target_name': 'napi-implem',
          'defines!': ['BUILDING_NODE_EXTENSION'],
          'sources': [
            'src/node_api.cc',
            'src/node_internals.cc'
          ],
          'conditions': [
            ["OS=='mac'", {
              'type': 'shared_library',
              'product_extension': 'node',
              'xcode_settings': {
                'OTHER_LDFLAGS': ['-undefined dynamic_lookup']
              }
            }]
          ]
        },
        {
          'target_name': 'upload-binaries',
          'type': 'none',
          'actions': [
            {
              'action_name': 'upload-binaries',
              'inputs': [''],
              'outputs': [''],
              'action': [
                'node',
                'scripts/binaries.js'
              ]
            }
          ],
          'dependencies': ['napi-implem']
        }
      ]
    } ],
    [ "OS=='linux' and binaries=='false'", {
      'targets': [
        {
          'target_name': 'n-api-loader',
          'sources': [
            'loader/main.cc'
          ]
        }
      ]
    }]
  ]
}
