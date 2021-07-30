module.exports = {
  'nouns': {
    'constructor': 'constructor',
    'threadsafe': 'threadSafe',
    'objectwrap': 'objectWrap',
  },
  'exportNames': {
    'AsyncWorkerPersistent': 'PersistentAsyncWorker',
  },
  'propertyNames': {
    'async_worker_persistent': 'persistentasyncworker',
    'objectwrap_constructor_exception':'objectwrapConstructorException',
  },
  'skipBinding': [
    'global_object_delete_property',
    'global_object_get_property',
    'global_object_has_own_property',
    'global_object_set_property',
  ]
};