#include <array>
#include "napi.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace tsfnutil {
template <typename TSFNWrapImpl, typename Context, typename TSFN>
class TSFNWrapBase : public ObjectWrap<TSFNWrapImpl> {
public:


  static void Init(Napi::Env env, Object exports, const std::string &ns) {
    // Get methods defined by child
    auto methods(TSFNWrapImpl::InstanceMethods());

    // Create a vector, since DefineClass doesn't accept arrays.
    std::vector<ClassPropertyDescriptor<TSFNWrapImpl>> methodsVec(methods.begin(), methods.end());

    auto locals(Object::New(env));
    locals.Set("TSFNWrap", ObjectWrap<TSFNWrapImpl>::DefineClass(env, "TSFNWrap", methodsVec));
    exports.Set(ns, locals);
  }

  // Release the TSFN. Returns a Promise that is resolved in the TSFN's
  // finalizer.
  // NOTE: the 'simple' test overrides this method, because it has no finalizer.
  Napi::Value Release(const CallbackInfo &info) {
    if (_deferred) {
      return _deferred->Promise();
    }

    auto env = info.Env();
    _deferred.reset(new Promise::Deferred(Promise::Deferred::New(env)));

    _tsfn.Release();
    return _deferred->Promise();
  };

  // TSFN finalizer. Resolves the Promise returned by `Release()` above.
  static void Finalizer(Napi::Env env,
                        std::unique_ptr<Promise::Deferred> *deferred,
                        Context * /*ctx*/) {
    if (deferred->get()) {
      (*deferred)->Resolve(Boolean::New(env, true));
      deferred->release();
    }
  }


  TSFNWrapBase(const CallbackInfo &callbackInfo)
      : ObjectWrap<TSFNWrapImpl>(callbackInfo) {}

protected:
  TSFN _tsfn;
  std::unique_ptr<Promise::Deferred> _deferred;
};

} // namespace tsfnutil

#endif
