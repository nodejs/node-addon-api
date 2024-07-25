# BasicEnv

The opaque data structure containing the environment in which the request is
being run.

The `Napi::BasicEnv` object is usually created and passed by the Node.js runtime
or node-addon-api infrastructure.

The `Napi::BasicEnv` object represents an environment that has a limited subset
of APIs when compared to `Napi::Env` and can be used in basic finalizers. See
[Finalization]() for more details.

## Methods

### Constructor

```cpp
Napi::BasicEnv::BasicEnv(node_api_nogc_env env);
```

- `[in] env`: The `node_api_nogc_env` environment from which to construct the
  `Napi::BasicEnv` object.

### node_api_nogc_env

```cpp
operator node_api_nogc_env() const;
```

Returns the `node_api_nogc_env` opaque data structure representing the
environment.

### GetInstanceData
```cpp
template <typename T> T* GetInstanceData() const;
```

Returns the instance data that was previously associated with the environment,
or `nullptr` if none was associated.

### SetInstanceData


```cpp
template <typename T> using Finalizer = void (*)(Env, T*);
template <typename T, Finalizer<T> fini = Env::DefaultFini<T>>
void SetInstanceData(T* data) const;
```

- `[template] fini`: A function to call when the instance data is to be deleted.
Accepts a function of the form `void CleanupData(Napi::Env env, T* data)`. If
not given, the default finalizer will be used, which simply uses the `delete`
operator to destroy `T*` when the addon instance is unloaded.
- `[in] data`: A pointer to data that will be associated with the instance of
the addon for the duration of its lifecycle.

Associates a data item stored at `T* data` with the current instance of the
addon. The item will be passed to the function `fini` which gets called when an
instance of the addon is unloaded.

### SetInstanceData

```cpp
template <typename DataType, typename HintType>
using FinalizerWithHint = void (*)(Env, DataType*, HintType*);
template <typename DataType,
          typename HintType,
          FinalizerWithHint<DataType, HintType> fini =
            Env::DefaultFiniWithHint<DataType, HintType>>
void SetInstanceData(DataType* data, HintType* hint) const;
```

- `[template] fini`: A function to call when the instance data is to be deleted.
Accepts a function of the form `void CleanupData(Napi::Env env, DataType* data,
HintType* hint)`. If not given, the default finalizer will be used, which simply
uses the `delete` operator to destroy `T*` when the addon instance is unloaded.
- `[in] data`: A pointer to data that will be associated with the instance of
the addon for the duration of its lifecycle.
- `[in] hint`: A pointer to data that will be associated with the instance of
the addon for the duration of its lifecycle and will be passed as a hint to
`fini` when the addon instance is unloaded.

Associates a data item stored at `T* data` with the current instance of the
addon. The item will be passed to the function `fini` which gets called when an
instance of the addon is unloaded. This overload accepts an additional hint to
be passed to `fini`.

### GetModuleFileName

```cpp
const char* Napi::Env::GetModuleFileName() const;
```

Returns a A URL containing the absolute path of the location from which the
add-on was loaded. For a file on the local file system it will start with
`file://`. The string is null-terminated and owned by env and must thus not be
modified or freed. It is only valid while the add-on is loaded.

### AddCleanupHook

```cpp
template <typename Hook>
CleanupHook<Hook> AddCleanupHook(Hook hook);
```

- `[in] hook`: A function to call when the environment exits. Accepts a function
  of the form `void ()`.

Registers `hook` as a function to be run once the current Node.js environment
exits. Unlike the underlying C-based Node-API, providing the same `hook`
multiple times **is** allowed. The hooks will be called in reverse order, i.e.
the most recently added one will be called first.

Returns an `Env::CleanupHook` object, which can be used to remove the hook via
its `Remove()` method.

### PostFinalizer

```cpp
using FinalizerWithoutData = void (*)(Env);

inline void PostFinalizer(FinalizerWithoutData finalizeCallback) const;
```

- `[in] finalizeCallback`: The function to queue for execution outside of the GC
  finalization, implementing `operator()(Napi::Env)`. See [Finalization]() for
  more details.

### PostFinalizer

```cpp
template <typename T>
using Finalizer = void (*)(Napi::Env, T*);

template <typename DataType>
inline void PostFinalizer(Finalizer<DataType> finalizeCallback, DataType* data) const;
```

- `[in] finalizeCallback`: The function to queue for execution outside of the GC
  finalization, implementing `operator()(Napi::Env, DataType*)`. See [Finalization]()
  for more details.
- `[in] data`: The data value passed to the `finalizeCallback` function.

### PostFinalizer

```cpp
template <typename DataType, typename HintType>
using FinalizerWithHint = void (*)(Napi::Env, DataType*, HintType*);

template <typename DataType, typename HintType>
inline void PostFinalizer(FinalizerWithHint<DataType, HintType> finalizeCallback,
                          DataType* data,
                          Hint* finalizeHint) const;
```

- `[in] finalizeCallback`: The function to queue for execution outside of the GC
  finalization, implementing `operator()(Napi::Env, DataType*, HintType*)`. See
  [Finalization]() for more details.
- `[in] data`: The data value passed to the `finalizeCallback` function.
- `[in] finalizeHint`: The hint value passed to the `finalizeCallback` function.

### AddCleanupHook

```cpp
template <typename Hook, typename Arg>
CleanupHook<Hook, Arg> AddCleanupHook(Hook hook, Arg* arg);
```

- `[in] hook`: A function to call when the environment exits. Accepts a function
  of the form `void (Arg* arg)`.
- `[in] arg`: A pointer to data that will be passed as the argument to `hook`.

Registers `hook` as a function to be run with the `arg` parameter once the
current Node.js environment exits. Unlike the underlying C-based Node-API,
providing the same `hook` and `arg` pair multiple times **is** allowed. The
hooks will be called in reverse order, i.e. the most recently added one will be
called first.

Returns an `Env::CleanupHook` object, which can be used to remove the hook via
its `Remove()` method.

# Env::CleanupHook

The `Env::CleanupHook` object allows removal of the hook added via
`Env::AddCleanupHook()`

## Methods

### IsEmpty

```cpp
bool IsEmpty();
```

Returns `true` if the cleanup hook was **not** successfully registered.

### Remove

```cpp
bool Remove(Env env);
```

Unregisters the hook from running once the current Node.js environment exits.

Returns `true` if the hook was successfully removed from the Node.js
environment.

[Finalization]: ./finalization.md
