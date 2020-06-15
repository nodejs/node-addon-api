#include "../util/util.h"
#include "napi.h"
#include <array>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

static constexpr auto DEFAULT_THREAD_COUNT = 10U;
static constexpr auto DEFAULT_CALL_COUNT = 2;

static struct {
  bool logCall = true;    // Uses JS console.log to output when the TSFN is
                          // processing the NonBlockingCall().
  bool logThread = false; // Uses native std::cout to output when the thread's
                          // NonBlockingCall() request has finished.
} DefaultOptions;         // Options from Start()

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

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace example {

class TSFNWrap;

// Context of the TSFN.
using Context = TSFNWrap;

// Data passed (as pointer) to [Non]BlockingCall
struct Data {
  std::promise<int> promise;
  uint32_t threadId;
  bool logCall;
  uint32_t base;
};
using DataType = std::unique_ptr<Data>;

// CallJs callback function
static void CallJs(Napi::Env env, Napi::Function jsCallback,
                   Context * /*context*/, DataType *dataPtr) {
  if (dataPtr != nullptr) {
    auto &data = *dataPtr;
    if (env != nullptr) {
      auto calculated = data->base * data->base;
      if (!jsCallback.IsEmpty()) {
        auto value = jsCallback.Call({Number::New(env, data->threadId), Number::New(env, calculated)});
        if (env.IsExceptionPending()) {
          const auto &error = env.GetAndClearPendingException();
          data->promise.set_exception(
              std::make_exception_ptr(std::runtime_error(error.Message())));
        } else if (value.IsNumber()) {
          calculated = value.ToNumber();
        }
      }
      if (data->logCall) {
        std::string message("Thread " + std::to_string(data->threadId) +
                            " CallJs resolving std::promise");
        auto console = env.Global().Get("console").As<Object>();
        console.Get("log").As<Function>().Call(console,
                                               {String::New(env, message)});
      }
      data->promise.set_value(calculated);
    } else {
      data->promise.set_exception(std::make_exception_ptr(
          std::runtime_error("TSFN has been finalized.")));
    }
  }
  // We do NOT delete data as it is a unique_ptr held by the calling thread.
}

// Full type of the ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<Context, DataType, CallJs>;

using base = tsfnutil::TSFNWrapBase<TSFNWrap, Context, TSFN>;

// A JS-accessible wrap that holds the TSFN.
class TSFNWrap : public base {
public:
  TSFNWrap(const CallbackInfo &info) : base(info) {
    if (info.Length() > 0 && info[0].IsObject()) {
      auto arg0 = info[0].ToObject();
      if (arg0.Has("cppExceptions")) {
        auto cppExceptions = arg0.Get("cppExceptions");
        if (cppExceptions.IsBoolean()) {
          cppExceptions = cppExceptions.As<Boolean>();
        } else {
          // We explicitly use the addon's except/noexcept settings here, since
          // we don't have a valid setting.
          Napi::TypeError::New(Env(), "cppExceptions is not a boolean")
              .ThrowAsJavaScriptException();
        }
      }
    }
  }
  ~TSFNWrap() {
    for (auto &thread : finalizerData->threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  struct FinalizerData {
    std::vector<std::thread> threads;
    std::unique_ptr<Promise::Deferred> deferred;
  };

  // The finalizer data is shared, because we want to join the threads if our
  // TSFNWrap object gets garbage-collected and there are still active threads.
  using FinalizerDataType = std::shared_ptr<FinalizerData>;

#define THREADLOG(X)                                                           \
  if (context->logThread) {                                                    \
    std::cout << X;                                                            \
  }
  static void threadEntry(size_t threadId, TSFN tsfn, uint32_t callCount,
                          Context *context) {
    using namespace std::chrono_literals;

    THREADLOG("Thread " << threadId << " starting...\n")

    for (auto i = 0U; i < callCount; ++i) {
      auto data = std::make_unique<Data>();
      data->base = threadId + 1;
      data->threadId = threadId;
      data->logCall = context->logCall;
      THREADLOG("Thread " << threadId << " making call, base = " << data->base
                          << "\n")

      tsfn.NonBlockingCall(&data);
      auto future = data->promise.get_future();
      auto result = future.get();
      context->callSucceeded(result);
      THREADLOG("Thread " << threadId << " got result: " << result << "\n")
    }

    THREADLOG("Thread " << threadId << " finished.\n\n")
    tsfn.Release();
  }
#undef THREADLOG

  static std::array<ClassPropertyDescriptor<TSFNWrap>, 4> InstanceMethods() {
    return {{InstanceMethod("getContext", &TSFNWrap::GetContext),
             InstanceMethod("start", &TSFNWrap::Start),
             InstanceMethod("callCount", &TSFNWrap::CallCount),
             InstanceMethod("release", &TSFNWrap::Release)}};
  }

  bool cppExceptions = false;
  bool logThread;
  bool logCall;
  std::atomic_uint succeededCalls;
  std::atomic_int aggregate;

  FinalizerDataType finalizerData;

  Napi::Value Start(const CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (_tsfn) {
      TSFN_THROW(this, Napi::Error::New(Env(), "TSFN already exists."),
                 Value());
    }

    // Creates a list to hold how many times each thread should make a call.
    std::vector<int32_t> callCounts;

    // The JS-provided callback to execute for each call (if provided)
    Function callback;

    finalizerData = std::make_shared<FinalizerData>();

    logThread = DefaultOptions.logThread;

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
              TSFN_THROW(this, Napi::TypeError::New(Env(), "Invalid arguments"),
                         Value());
            }
          }
        } else if (threads.IsNumber()) {
          auto threadCount = threads.As<Number>().Int32Value();
          for (auto i = 0; i < threadCount; ++i) {
            callCounts.push_back(DEFAULT_CALL_COUNT);
          }
        } else {
          TSFN_THROW(this, Napi::TypeError::New(Env(), "Invalid arguments"),
                     Value());
        }
      }

      if (arg0.Has("callback")) {
        auto cb = arg0.Get("callback");
        if (cb.IsFunction()) {
          callback = cb.As<Function>();
        } else {
          TSFN_THROW(this,
                     Napi::TypeError::New(Env(), "Callback is not a function"),
                     Value());
        }
      }

      if (arg0.Has("logCall")) {
        auto logCallOption = arg0.Get("logCall");
        if (logCallOption.IsBoolean()) {
          logCall = logCallOption.As<Boolean>();
        } else {
          TSFN_THROW(this,
                     Napi::TypeError::New(Env(), "logCall is not a boolean"),
                     Value());
        }
      }

      if (arg0.Has("logThread")) {
        auto logThreadOption = arg0.Get("logThread");
        if (logThreadOption.IsBoolean()) {
          logThread = logThreadOption.As<Boolean>();
        } else {
          TSFN_THROW(this,
                     Napi::TypeError::New(Env(), "logThread is not a boolean"),
                     Value());
        }
      }
    }

    // Apply default arguments
    if (callCounts.size() == 0) {
      for (auto i = 0U; i < DEFAULT_THREAD_COUNT; ++i) {
        callCounts.push_back(DEFAULT_CALL_COUNT);
      }
    }

    const auto threadCount = callCounts.size();

    auto *finalizerDataPtr = new FinalizerDataType(finalizerData);

    succeededCalls = 0;
    aggregate = 0;
    _tsfn = TSFN::New(
        env,                                  // napi_env env,
        TSFN::FunctionOrEmpty(env, callback), // const Function& callback,
        Value(),                              // const Object& resource,
        "Test",                               // ResourceString resourceName,
        0,                                    // size_t maxQueueSize,
        threadCount + 1, // size_t initialThreadCount, +1 for Node thread
        this,            // Context* context,
        Finalizer,       // Finalizer finalizer
        finalizerDataPtr // FinalizerDataType* data
    );

    for (auto threadId = 0U; threadId < threadCount; ++threadId) {
      finalizerData->threads.push_back(std::thread(threadEntry, threadId, _tsfn,
                                                   callCounts[threadId], this));
    }

    return Number::New(env, threadCount);
  };

  // TSFN finalizer. Joins the threads and resolves the Promise returned by
  // `Release()` above.
  static void Finalizer(Napi::Env env, FinalizerDataType *finalizeData,
                        Context *ctx) {

    if (ctx->logThread) {
      std::cout << "Finalizer joining threads\n";
    }
    for (auto &thread : (*finalizeData)->threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
    ctx->clearTSFN();
    if (ctx->logThread) {
      std::cout << "Finished finalizing threads.\n";
    }

    (*finalizeData)->deferred->Resolve(Boolean::New(env, true));
    delete finalizeData;
  }

  Napi::Value Release(const CallbackInfo &info) {
    if (finalizerData->deferred) {
      return finalizerData->deferred->Promise();
    }
    finalizerData->deferred.reset(
        new Promise::Deferred(Promise::Deferred::New(info.Env())));
    _tsfn.Release();
    return finalizerData->deferred->Promise();
  };

  Napi::Value CallCount(const CallbackInfo &info) {
    Napi::Env env(info.Env());

    auto results = Array::New(env, 2);
    results.Set("0", Number::New(env, succeededCalls));
    results.Set("1", Number::New(env, aggregate));
    return results;
  };

  Napi::Value GetContext(const CallbackInfo &) {
    return _tsfn.GetContext()->Value();
  };

  // This method does not run on the Node thread.
  void clearTSFN() { _tsfn = TSFN(); }

  // This method does not run on the Node thread.
  void callSucceeded(int result) {
    succeededCalls++;
    aggregate += result;
  }
};
} // namespace example

Object InitThreadSafeFunctionExExample(Env env) {
  auto exports(Object::New(env));
  example::TSFNWrap::Init(env, exports, "example");
  return exports;
}

#endif
