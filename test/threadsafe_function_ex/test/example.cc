/**
 * This test is programmatically represents the example shown in
 * `doc/threadsafe_function_ex.md`
 */

#if (NAPI_VERSION > 3)

#include "napi.h"
#include <thread>
#include <vector>
static constexpr size_t DEFAULT_THREAD_COUNT = 10;
static constexpr int32_t DEFAULT_CALL_COUNT = 2;

/**
 * @brief Macro used specifically to support the dual CI test / documentation
 * example setup. Exceptions are always thrown as JavaScript exceptions when
 * running in example mode.
 *
 */
#define TSFN_THROW(tsfnWrap, e, ...)                                           \
  if (tsfnWrap->cppExceptions) {                                               \
    do {                                                                       \
      (e).ThrowAsJavaScriptException();                                        \
      return __VA_ARGS__;                                                      \
    } while (0);                                                               \
  } else {                                                                     \
    NAPI_THROW(e, __VA_ARGS__);                                                \
  }

using namespace Napi;

namespace {

// Context of our TSFN.
struct Context {
  int32_t threadId;
};

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

struct FinalizerDataType {
  std::vector<std::thread> threads;
};

// A JS-accessible wrap that holds a TSFN.
class TSFNWrap : public ObjectWrap<TSFNWrap> {

public:
  static Object Init(Napi::Env env, Object exports);
  TSFNWrap(const CallbackInfo &info);

  // When running as an example, we want exceptions to always go to JavaScript,
  // allowing the user to try/catch errors from the addon.
  bool cppExceptions;

private:
  Napi::Value Start(const CallbackInfo &info);
  Napi::Value Release(const CallbackInfo &info);

  // Instantiated by `Start`; resolved on finalize of tsfn.
  Promise::Deferred _deferred;

  // Reference to our TSFN
  TSFN _tsfn;

  // Object.prototype.toString reference for use with error messages
  FunctionReference _toString;
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

static void threadEntry(size_t threadId, TSFN tsfn, int32_t callCount) {
  using namespace std::chrono_literals;
  for (int32_t i = 0; i < callCount; ++i) {
    tsfn.NonBlockingCall(new int);
    std::this_thread::sleep_for(50ms * threadId);
  }
  tsfn.Release();
}

/**
 * @brief Construct a new TSFNWrap object on the main thread. If any arguments
 * are passed, exceptions in the addon will always be thrown JavaScript
 * exceptions, allowing the user to try/catch errors from the addon.
 *
 * @param info
 */
TSFNWrap::TSFNWrap(const CallbackInfo &info)
    : ObjectWrap<TSFNWrap>(info),
      _deferred(Promise::Deferred::New(info.Env())) {
  auto env = info.Env();
  _toString = Napi::Persistent(env.Global()
                                   .Get("Object")
                                   .ToObject()
                                   .Get("prototype")
                                   .ToObject()
                                   .Get("toString")
                                   .As<Function>());
  cppExceptions = true;
  info.Length() > 0;
}
} // namespace

/**
 * @brief Instance method `TSFNWrap#start`
 *
 * @param info
 * @return undefined
 */
Napi::Value TSFNWrap::Start(const CallbackInfo &info) {
  Napi::Env env = info.Env();

  // Creates a list to hold how many times each thread should make a call.
  std::vector<int32_t> callCounts;

  // The JS-provided callback to execute for each call (if provided)
  Function callback;

  if (info.Length() > 0 && info[0].IsObject()) {
    auto arg0 = info[0].ToObject();
    if (arg0.Has("threads")) {
      Napi::Value threads = arg0.Get("threads");
      if (threads.IsArray()) {
        Napi::Array threadsArray = threads.As<Napi::Array>();
        for (auto i = 0U; i < threadsArray.Length(); ++i) {
          Napi::Value elem = threadsArray.Get(i);
          if (elem.IsNumber()) {
            callCounts.push_back(elem.As<Number>().Int32Value());
          } else {
            // TSFN_THROW(this,
            //   Napi::TypeError::New(Env(),
            //   "Invalid arguments"),
            //   Object());

            // ThrowAsJavaScriptException
            Napi::TypeError::New(Env(), "Invalid arguments")
                .ThrowAsJavaScriptException();
            return env.Undefined();

            // if (this->cppExceptions) {
            //   do {
            //     (Napi::TypeError::New(Env(), "Invalid arguments"))
            //         .ThrowAsJavaScriptException();
            //     return Object();
            //   } while (0);
            // } else {
            //   NAPI_THROW(Napi::TypeError::New(Env(), "Invalid arguments"),
            //              Object());
            // }
          }
        }
      } else if (threads.IsNumber()) {
        auto threadCount = threads.As<Number>().Int32Value();
        for (int32_t i = 0; i < threadCount; ++i) {
          callCounts.push_back(DEFAULT_CALL_COUNT);
        }
      } else {
        TSFN_THROW(this, Napi::TypeError::New(Env(), "Invalid arguments"),
                   Number());
      }
    }

    if (arg0.Has("callback")) {
      auto cb = arg0.Get("callback");
      if (cb.IsFunction()) {
        callback = cb.As<Function>();
      } else {
        TSFN_THROW(this,
                   Napi::TypeError::New(Env(), "Callback is not a function"),
                   Number());
      }
    }
  }

  // Apply default arguments
  if (callCounts.size() == 0) {
    for (size_t i = 0; i < DEFAULT_THREAD_COUNT; ++i) {
      callCounts.push_back(DEFAULT_CALL_COUNT);
    }
  }

  // const auto threadCount = callCounts.size();
  // FinalizerDataType *finalizerData = new FinalizerDataType();
  // // TSFN::New(info.Env(), info[0].As<Function>(), Object::New(info.Env()),
  // //     "Test", tsfnInfo.maxQueueSize, 2, &tsfnInfo, JoinTheThreads,
  
  // threads); _tsfn = TSFN::New(env,      // napi_env env,
  //                   callback, // const Function& callback,
  //                   "Test",   // ResourceString resourceName,
  //                   0,        // size_t maxQueueSize,
  //                   threadCount         // size_t initialThreadCount,
  // );
  // for (int32_t threadId = 0; threadId < threadCount; ++threadId) {
  //   // static void threadEntry(size_t threadId, TSFN tsfn, int32_t callCount)
  //   {

  //   finalizerData->threads.push_back(
  //       std::thread(threadEntry, threadId, _tsfn, callCounts[threadId]));
  // }
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
