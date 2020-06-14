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
  } DefaultOptions;                // Options from Start()

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

using CompletionHandler = std::function<void()>;

// Data passed (as pointer) to [Non]BlockingCall
struct DataType {
  // Promise::Deferred;
  // CompletionHandler handler;
  std::future<int> deferred;
};

// CallJs callback function
static void CallJs(Napi::Env env, Napi::Function /*jsCallback*/,
                   Context *context, DataType *data) {
  if (env != nullptr) {
    if (data != nullptr) {
      // data->Resolve(context->Value());
    }
  }
  if (data != nullptr) {
    delete data;
  }
}

// Full type of the ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<Context, DataType, CallJs>;

struct FinalizerDataType {
  std::vector<std::thread> threads;
  std::unique_ptr<Promise::Deferred> deferred;
  // struct {
  //   bool logCall = true;    // Uses JS console.log to output when the TSFN is
  //                           // processing the NonBlockingCall().
  //   bool logThread = false; // Uses native std::cout to output when the thread's
  //                           // NonBlockingCall() request has finished.
  // } options;                // Options from Start()
};

static void threadEntry(size_t threadId, TSFN tsfn, uint32_t callCount,
                        bool logThread) {
  using namespace std::chrono_literals;
  for (auto i = 0U; i < callCount; ++i) {
    // auto callData = new DataType();
    // tsfn.NonBlockingCall(callData);
    // auto result = callData->deferred.get();
    // if (logThread) {
      // std::cout << "Thread " << threadId << " got result " << result << "\n";
    // }

    // std::this_thread::sleep_for(50ms * threadId);
  }
  std::cout << "Thread " << threadId << "finished\n";
  tsfn.Release();
}

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
    for (auto& thread : finalizerData->threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  static std::array<ClassPropertyDescriptor<TSFNWrap>, 3> InstanceMethods() {
    return {InstanceMethod("call", &TSFNWrap::Call),
            InstanceMethod("start", &TSFNWrap::Start),
            InstanceMethod("release", &TSFNWrap::Release)};
  }

  bool cppExceptions = false;
  std::shared_ptr<FinalizerDataType> finalizerData;

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

    // std::unique_ptr<FinalizerDataType> finalizerData = 
        // std::make_unique<FinalizerDataType>();
    
    // finalizerData = std::shared_ptr<FinalizerDataType>(FinalizerDataType{ std::vector<std::thread>() , Promise::Deferred::New(env) });

    finalizerData = std::make_shared<FinalizerDataType>();


    bool logThread = DefaultOptions.logThread;
    bool logCall = DefaultOptions.logCall;

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

    auto *finalizerDataPtr = new std::shared_ptr<FinalizerDataType>(finalizerData);

    _tsfn = TSFN::New(
        env,                               // napi_env env,
        TSFN::DefaultFunctionFactory(env), // const Function& callback,
        Value(),                           // const Object& resource,
        "Test",                            // ResourceString resourceName,
        0,                                 // size_t maxQueueSize,
        threadCount + 1,        // size_t initialThreadCount, +1 for Node thread
        this,                   // Context* context,
        Finalizer,              // Finalizer finalizer
        finalizerDataPtr // FinalizerDataType* data
    );

    for (auto threadId = 0U; threadId < threadCount; ++threadId) {
      finalizerData->threads.push_back(
          std::thread(threadEntry, threadId, _tsfn, callCounts[threadId],
                      logThread));
    }


    return String::New(env, "started");
  };

  // TSFN finalizer. Resolves the Promise returned by `Release()` above.
  static void Finalizer(Napi::Env env, std::shared_ptr<FinalizerDataType> *finalizeData,
                        Context *ctx) {
    // for (auto thread : finalizeData->threads) {

    for (auto &thread : (*finalizeData)->threads) {
      std::cout << "Finalizer joining thread\n";
      if (thread.joinable()) {
        thread.join();
      }
    }

    delete finalizeData;

    // }
    // if (deferred->get()) {
    //   (*deferred)->Resolve(Boolean::New(env, true));
    //   deferred->release();

    // }
  }

  Napi::Value Release(const CallbackInfo &info) {
    if (finalizerData->deferred) {
      return finalizerData->deferred->Promise();
    }
    // return finalizerData->deferred.Promise();
    auto env = info.Env();
    finalizerData->deferred.reset(new Promise::Deferred(Promise::Deferred::New(env)));
    _tsfn.Release();
    return finalizerData->deferred->Promise();
  };

  Napi::Value Call(const CallbackInfo &info) {
    // auto *callData = new DataType(info.Env());
    // _tsfn.NonBlockingCall(callData);
    // return callData->Promise();
    return info.Env().Undefined();
  };

  Napi::Value GetContext(const CallbackInfo &) {
    return _tsfn.GetContext()->Value();
  };
};
} // namespace example

Object InitThreadSafeFunctionExExample(Env env) {
  auto exports(Object::New(env));
  example::TSFNWrap::Init(env, exports, "example");
  return exports;
}

#endif
