# VersionManagement

The `Napi::VersionManagement` class contains methods that allow to retrieve 
informations about the version of N-API and Node.js. It is important take 
decisions based on different versions of the system.

## Methods

### GetNapiVersion

Retrieves the highest N-API version supported by Node.js runtime.

```cpp
static uint32_t GetNapiVersion(Env env);
```

- `[in] env`: The environment in which the API is invoked under.

Returns the highest N-API version supported by Node.js runtime.

### GetNodeVersion

Retrives information about Node.js version present on the system. All the 
informations are store in the `napi_node_version` structrue that is defined as 
repoted below:

```cpp
typedef struct {
  uint32_t major;
  uint32_t minor;
  uint32_t patch;
  const char* release;
} napi_node_version;
````

```cpp
static const napi_node_version* GetNodeVersion(Env env);
```

- `[in] env`: The environment in which the API is invoked under.

Returns the structure a pointer to the structure `napi_node_version` populated by
the version information of Node.js runtime.

