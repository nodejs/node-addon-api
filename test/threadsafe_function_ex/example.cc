/**
 * This test is programmatically represents the example shown in
 * `doc/threadsafe_function_ex.md`
 */

#include "napi.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace {

// Context of our TSFN.
struct Context {};

// Data passed (as pointer) to ThreadSafeFunctionEx::[Non]BlockingCall
using DataType = int;

// Callback function
static void Callback(Napi::Env env, Napi::Function jsCallback, Context *context,
                     DataType *data) {
  // Check that the threadsafe function has not been finalized. Node calls this
  // callback for items remaining on the queue once finalization has completed.
  if (!(env == nullptr || jsCallback == nullptr)) {
  }
  if (data != nullptr) {
    delete data;
  }
}

// Full type of our ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<Context, DataType, Callback>;

// A JS-accessible wrap that holds a TSFN.
class TSFNWrap : public ObjectWrap<TSFNWrap> {

public:
  static Object Init(Napi::Env env, Object exports);
  TSFNWrap(const CallbackInfo &info);

private:
  Napi::Value Start(const CallbackInfo &info);
  Napi::Value Release(const CallbackInfo &info);
};

/**
 * @brief Initialize `TSFNWrap` on the environment.
 *
 * @param env
 * @param exports
 * @return Object
 */
Object TSFNWrap::Init(Napi::Env env, Object exports) {
  Function func = DefineClass(env, "TSFNWrap",
                              {InstanceMethod("start", &TSFNWrap::Start),
                               InstanceMethod("release", &TSFNWrap::Release)});

  exports.Set("TSFNWrap", func);
  return exports;
}

/**
 * @brief Construct a new TSFNWrap::TSFNWrap object
 *
 * @param info
 */
TSFNWrap::TSFNWrap(const CallbackInfo &info) : ObjectWrap<TSFNWrap>(info) {}
} // namespace

/**
 * @brief Instance method `TSFNWrap#start`
 *
 * @param info
 * @return undefined
 */
Napi::Value TSFNWrap::Start(const CallbackInfo &info) {
  Napi::Env env = info.Env();
  return env.Undefined();
};

/**
 * @brief Instance method `TSFNWrap#release`
 *
 * @param info
 * @return undefined
 */
Napi::Value TSFNWrap::Release(const CallbackInfo &info) {
  Napi::Env env = info.Env();
  return env.Undefined();
};

/**
 * @brief Module initialization function
 *
 * @param env
 * @return Object
 */
Object InitThreadSafeFunctionExExample(Env env) {
  return TSFNWrap::Init(env, Object::New(env));
}

#endif
