{
  "variables": {
    "runningFromPrebuild": "<!(node -p \"process.env.RUNNING_FROM_PREBUILD\")"
  },
  "targets": [
    {
      "target_name": "binding-gyp-must-have-at-least-one-target",
      "type": "none"
    }
  ],
  "conditions": [
    [ "runningFromPrebuild=='true'",
      {
        "targets+": [
          {
            "target_name": "n-api-implem",
            "sources": [
              "src/node_api.cc",
              "src/node_internals.cc"
            ],
            "defines": ["EXTERNAL_NAPI"]
          }
        ],
      }
    ],
    [ "OS!='win' and runningFromPrebuild!='true'",
      {
        "targets+": [
          {
            "target_name": "n-api-loader",
            "sources": [ "loader/main.c" ]
          }
        ]
      }
    ]
  ]
}
