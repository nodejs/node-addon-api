#include <node_api.h>
#include "napi.h"

// This is a simple wrapper around Node-API that will allow us to inject
// additional steps (such as type checks) before calling into it. For now, we
// merely partition the API into two parts: pure and non-pure, where pure APIs
// can be called from anywhere in the add-on's code, and non-pure APIs cannot
// be called from within finalizers.

namespace NodeApi {

#define DECLARE_API_WRAPPER(c_api, wrapper_name, env_type)                     \
  template <typename... Args>                                                  \
  inline napi_status wrapper_name(env_type env, Args... rest) {                \
    return c_api(env, rest...);                                                \
  }

#define DECLARE_NON_PURE_API(c_api, wrapper_name)                              \
  DECLARE_API_WRAPPER(c_api, wrapper_name, Napi::NonPureEnv)

#define NON_PURE_APIS(V)                                                       \
  V(napi_add_async_cleanup_hook, AddAsyncCleanupHook)                          \
  V(napi_add_finalizer, AddFinalizer)                                          \
  V(napi_async_destroy, AsyncDestroy)                                          \
  V(napi_async_init, AsyncInit)                                                \
  V(napi_call_function, CallFunction)                                          \
  V(napi_check_object_type_tag, CheckObjectTypeTag)                            \
  V(napi_close_escapable_handle_scope, CloseEscapableHandleScope)              \
  V(napi_close_handle_scope, CloseHandleScope)                                 \
  V(napi_coerce_to_bool, CoerceToBool)                                         \
  V(napi_coerce_to_number, CoerceToNumber)                                     \
  V(napi_coerce_to_object, CoerceToObject)                                     \
  V(napi_coerce_to_string, CoerceToString)                                     \
  V(napi_create_array, CreateArray)                                            \
  V(napi_create_array_with_length, CreateArrayWithLength)                      \
  V(napi_create_arraybuffer, CreateArrayBuffer)                                \
  V(napi_create_async_work, CreateAsyncWork)                                   \
  V(napi_create_bigint_int64, CreateBigintInt64)                               \
  V(napi_create_bigint_uint64, CreateBigintUint64)                             \
  V(napi_create_bigint_words, CreateBigintWords)                               \
  V(napi_create_buffer, CreateBuffer)                                          \
  V(napi_create_buffer_copy, CreateBufferCopy)                                 \
  V(napi_create_dataview, CreateDataView)                                      \
  V(napi_create_date, CreateDate)                                              \
  V(napi_create_double, CreateDouble)                                          \
  V(napi_create_error, CreateError)                                            \
  V(napi_create_external, CreateExternal)                                      \
  V(napi_create_external_arraybuffer, CreateExternalArrayBuffer)               \
  V(napi_create_external_buffer, CreateExternalBuffer)                         \
  V(napi_create_function, CreateFunction)                                      \
  V(napi_create_int32, CreateInt32)                                            \
  V(napi_create_int64, CreateInt64)                                            \
  V(napi_create_object, CreateObject)                                          \
  V(napi_create_promise, CreatePromise)                                        \
  V(napi_create_range_error, CreateRangeError)                                 \
  V(napi_create_reference, CreateReference)                                    \
  V(napi_create_string_latin1, CreateStringLatin1)                             \
  V(napi_create_string_utf16, CreateStringUtf16)                               \
  V(napi_create_string_utf8, CreateStringUtf8)                                 \
  V(napi_create_symbol, CreateSymbol)                                          \
  V(napi_create_threadsafe_function, CreateThreadsafeFunction)                 \
  V(napi_create_type_error, CreateTypeError)                                   \
  V(napi_create_typedarray, CreateTypedArray)                                  \
  V(napi_create_uint32, CreateUint32)                                          \
  V(napi_define_class, DefineClass)                                            \
  V(napi_define_properties, DefineProperties)                                  \
  V(napi_delete_async_work, DeleteAsyncWork)                                   \
  V(napi_delete_element, DeleteElement)                                        \
  V(napi_delete_property, DeleteProperty)                                      \
  V(napi_delete_reference, DeleteReference)                                    \
  V(napi_detach_arraybuffer, DetachArrayBuffer)                                \
  V(napi_escape_handle, EscapeHandle)                                          \
  V(napi_fatal_exception, FatalException)                                      \
  V(napi_get_all_property_names, GetAllPropertyNames)                          \
  V(napi_get_and_clear_last_exception, GetAndClearLastException)               \
  V(napi_get_array_length, GetArrayLength)                                     \
  V(napi_get_arraybuffer_info, GetArrayBufferInfo)                             \
  V(napi_get_boolean, GetBoolean)                                              \
  V(napi_get_buffer_info, GetBufferInfo)                                       \
  V(napi_get_dataview_info, GetDataViewInfo)                                   \
  V(napi_get_date_value, GetDateValue)                                         \
  V(napi_get_element, GetElement)                                              \
  V(napi_get_global, GetGlobal)                                                \
  V(napi_get_named_property, GetNamedProperty)                                 \
  V(napi_get_new_target, GetNewTarget)                                         \
  V(napi_get_null, GetNull)                                                    \
  V(napi_get_property, GetProperty)                                            \
  V(napi_get_property_names, GetPropertyNames)                                 \
  V(napi_get_prototype, GetPrototype)                                          \
  V(napi_get_reference_value, GetReferenceValue)                               \
  V(napi_get_typedarray_info, GetTypedArrayInfo)                               \
  V(napi_get_undefined, GetUndefined)                                          \
  V(napi_get_value_bigint_int64, GetValueBigintInt64)                          \
  V(napi_get_value_bigint_uint64, GetValueBigintUint64)                        \
  V(napi_get_value_bigint_words, GetValueBigintWords)                          \
  V(napi_get_value_bool, GetValueBool)                                         \
  V(napi_get_value_double, GetValueDouble)                                     \
  V(napi_get_value_external, GetValueExternal)                                 \
  V(napi_get_value_int32, GetValueInt32)                                       \
  V(napi_get_value_int64, GetValueInt64)                                       \
  V(napi_get_value_string_latin1, GetValueStringLatin1)                        \
  V(napi_get_value_string_utf16, GetValueStringUtf16)                          \
  V(napi_get_value_string_utf8, GetValueStringUtf8)                            \
  V(napi_get_value_uint32, GetValueUint32)                                     \
  V(napi_has_element, HasElement)                                              \
  V(napi_has_named_property, HasNamedProperty)                                 \
  V(napi_has_own_property, HasOwnProperty)                                     \
  V(napi_has_property, HasProperty)                                            \
  V(napi_instanceof, Instanceof)                                               \
  V(napi_is_array, IsArray)                                                    \
  V(napi_is_arraybuffer, IsArrayBuffer)                                        \
  V(napi_is_buffer, IsBuffer)                                                  \
  V(napi_is_dataview, IsDataView)                                              \
  V(napi_is_date, IsDate)                                                      \
  V(napi_is_detached_arraybuffer, IsDetachedArrayBuffer)                       \
  V(napi_is_error, IsError)                                                    \
  V(napi_is_exception_pending, IsExceptionPending)                             \
  V(napi_is_promise, IsPromise)                                                \
  V(napi_is_typedarray, IsTypedArray)                                          \
  V(napi_make_callback, MakeCallback)                                          \
  V(napi_new_instance, NewInstance)                                            \
  V(napi_object_freeze, ObjectFreeze)                                          \
  V(napi_object_seal, ObjectSeal)                                              \
  V(napi_open_escapable_handle_scope, OpenEscapableHandleScope)                \
  V(napi_open_handle_scope, OpenHandleScope)                                   \
  V(napi_reference_ref, ReferenceRef)                                          \
  V(napi_reference_unref, ReferenceUnref)                                      \
  V(napi_reject_deferred, RejectDeferred)                                      \
  V(napi_remove_wrap, RemoveWrap)                                              \
  V(napi_resolve_deferred, ResolveDeferred)                                    \
  V(napi_run_script, RunScript)                                                \
  V(napi_set_element, SetElement)                                              \
  V(napi_set_named_property, SetNamedProperty)                                 \
  V(napi_set_property, SetProperty)                                            \
  V(napi_strict_equals, StrictEquals)                                          \
  V(napi_throw, Throw)                                                         \
  V(napi_throw_error, ThrowError)                                              \
  V(napi_throw_range_error, ThrowRangeError)                                   \
  V(napi_throw_type_error, ThrowTypeError)                                     \
  V(napi_type_tag_object, TypeTagObject)                                       \
  V(napi_typeof, Typeof)                                                       \
  V(napi_unwrap, Unwrap)                                                       \
  V(napi_wrap, Wrap)                                                           \
  V(node_api_create_external_string_latin1, CreateExternalStringLatin1)        \
  V(node_api_create_external_string_utf16, CreateExternalStringUtf16)          \
  V(node_api_create_syntax_error, CreateSyntaxError)                           \
  V(node_api_symbol_for, SymbolFor)                                            \
  V(node_api_throw_syntax_error, ThrowSyntaxError)

NON_PURE_APIS(DECLARE_NON_PURE_API)

#define DECLARE_PURE_API(c_api, wrapper_name)                                  \
  DECLARE_API_WRAPPER(c_api, wrapper_name, Napi::PureEnv)

#define PURE_APIS(V)                                                           \
  V(napi_acquire_threadsafe_function, AcquireThreadsafeFunction)               \
  V(napi_add_env_cleanup_hook, AddEnvCleanupHook)                              \
  V(napi_adjust_external_memory, AdjustExternalMemory)                         \
  V(napi_cancel_async_work, CancelAsyncWork)                                   \
  V(napi_close_callback_scope, CloseCallbackScope)                             \
  V(napi_get_cb_info, GetCbInfo)                                               \
  V(napi_get_instance_data, GetInstanceData)                                   \
  V(napi_get_last_error_info, GetLastErrorInfo)                                \
  V(napi_get_node_version, GetNodeVersion)                                     \
  V(napi_get_uv_event_loop, GetUvEventLoop)                                    \
  V(napi_get_version, GetVersion)                                              \
  V(napi_open_callback_scope, OpenCallbackScope)                               \
  V(napi_queue_async_work, QueueAsyncWork)                                     \
  V(napi_ref_threadsafe_function, RefThreadsafeFunction)                       \
  V(napi_remove_async_cleanup_hook, RemoveAsyncCleanupHook)                    \
  V(napi_remove_env_cleanup_hook, RemoveEnvCleanupHook)                        \
  V(napi_set_instance_data, SetInstanceData)                                   \
  V(napi_unref_threadsafe_function, UnrefThreadsafeFunction)                   \
  V(node_api_get_module_file_name, GetModuleFileName)                          \
  V(node_api_post_finalizer, PostFinalizer)

PURE_APIS(DECLARE_PURE_API)

}  // end of namespace NodeApi
