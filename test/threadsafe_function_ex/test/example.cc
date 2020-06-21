#include "../util/util.h"
#include "napi.h"
#include <array>
#include <future>
#include <iostream>
#include <set>
#include <thread>
#include <vector>

using ThreadExitHandler = void (*)(size_t threadId);

struct ThreadOptions {
  size_t threadId;
  int calls;
  int callDelay;
  int threadDelay;
};

static struct {
  bool logCall = true;    // Uses JS console.log to output when the TSFN is
                          // processing the NonBlockingCall().
  bool logThread = false; // Uses native std::cout to output when the thread's
                          // NonBlockingCall() request has finished.
} DefaultOptions;         // Options from Start()

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace example {

class TSFNWrap;

// Context of the TSFN.
using Context = TSFNWrap;

// Data returned to a thread when it requests a TSFN call. This example uses
// promises to synchronize between threads. Since this example needs to be built
// with exceptions both enabled and disabled, we will always use a static
// "positive" result, and dynamically at run-time determine if it failed.
// Otherwise, we could use std::promise.set_exception to handle errors.
struct CallJsResult {
  int result;
  bool isFinalized;
  std::string error;
};

// The structure of data we send to `CallJs`
struct Data {
  std::promise<CallJsResult> promise;
  uint32_t threadId;
  bool logCall;
  int callDelay;
  uint32_t base; // The "input" data, which CallJs will calculate `base * base`
  int callId;    // The call id (unique to the thread)
};

// Data passed (as pointer) to [Non]BlockingCall is shared among multiple
// threads (native thread and Node thread).
using DataType = std::shared_ptr<Data>;

// When providing the `CallJs` result back to the thread, we pass information
// about where and how the result came (for logging).
enum ResultLocation {
  NUMBER,  // The callback returned a number
  PROMISE, // The callback returned a Promise that resolved to a number
  DEFAULT  // There was no callback provided to TSFN::New
};

// CallJs callback function, used to transform the native C data to JS data.
static void CallJs(Napi::Env env, Napi::Function jsCallback,
                   Context * /*context*/, DataType *dataPtr) {
  // If we have data
  if (dataPtr != nullptr) {
    std::weak_ptr<Data> weakData(*dataPtr);
    // Create concrete reference to our DataType.
    auto &data(*dataPtr);

    // The success handler ran by the following `CallJs` function.
    auto handleResult = [=](Napi::Env env, int calculated,
                            ResultLocation location) {
      // auto &data(*dataPtr);
      if (auto data =
              weakData
                  .lock()) { // Has to be copied into a shared_ptr before usage
        // std::cout << *data << "\n";
        if (data->logCall) {
          std::string message(
              "[Thread " + std::to_string(data->threadId) +
              "] [CallJs  ] [Call " + std::to_string(data->callId) +
              "] Receive answer: result = " + std::to_string(calculated) +
              (location == ResultLocation::NUMBER
                   ? " (as number)"
                   : location == ResultLocation::PROMISE ? " (as Promise)"
                                                         : " (as default)"));

          auto console = env.Global().Get("console").As<Object>();
          console.Get("log").As<Function>().Call(console,
                                                 {String::New(env, message)});
        }
        // Resolve the `std::promise` awaited on in the child thread.
        data->promise.set_value(CallJsResult{calculated, false, ""});
        // Free the data.
        // delete dataPtr;
      }
    };

    // The error handler ran by the following `CallJs` function.

    auto handleError = [=](const std::string &what) {
      if (auto data =
              weakData
                  .lock()) { // Has to be copied into a shared_ptr before usage
        // Resolve the `std::promise` awaited on in the child thread with an
        // "errored success" value. Instead of erroring at the thread level,
        // this could also return the default result.
        data->promise.set_value(CallJsResult{0, false, what});
      }

      // // Free the data.
      // delete dataPtr;
    };

    if (env != nullptr) {
      // If the callback was provided at construction time via TSFN::New
      if (!jsCallback.IsEmpty()) {
        // Call the callback
        auto value = jsCallback.Call(
            {Number::New(env, data->threadId), Number::New(env, data->callId),
             Number::New(env, data->logCall), Number::New(env, data->base),
             Number::New(env, data->callDelay)});

        // Check if the callback failed
        if (env.IsExceptionPending()) {
          const auto &error = env.GetAndClearPendingException();
          handleError(error.Message());
        }

        // Check for an immediate number result
        else if (value.IsNumber()) {
          handleResult(env, value.ToNumber(), ResultLocation::NUMBER);
        }

        // Check for a Promise<T> result
        else if (value.IsPromise()) {

          // Construct the Promise.then and Promise.catch handlers. These could
          // also be a statically-defined `Function`s.

          // Promise.then handler.
          auto promiseHandlerThen = Function::New(env, [=](const CallbackInfo
                                                               &info) {
            // Check for Promise<T = number> result
            if (info.Length() < 1 || !info[0].IsNumber()) {
              handleError(
                  "Expected callback Promise resolution to be of type number");
            } else {
              auto result = info[0].ToNumber().Int32Value();
              handleResult(info.Env(), result, ResultLocation::PROMISE);
            }
          });

          // Promise.catch handler.
          auto promiseHandlerCatch =
              Function::New(env, [&](const CallbackInfo &info) {
                if (info.Length() < 1 || !info[0].IsObject()) {
                  handleError("Unknown error in callback handler");
                } else {
                  auto errorAsValue(info[0]
                                        .As<Object>()
                                        .Get("toString")
                                        .As<Function>()
                                        .Call(info[0], {}));
                  handleError(errorAsValue.ToString());
                }
              });

          // Execute the JavaScript equivalent of `promise.then.call(promise,
          // promiseHandlerThen).catch.call(promise, promiseHandlerCatch);`
          value.As<Promise>()
              .Get("then")
              .As<Function>()
              .Call(value, {promiseHandlerThen})
              .As<Promise>()
              .Get("catch")
              .As<Function>()
              .Call(value, {promiseHandlerCatch});
        }
        // When using N-API 4, the callback is a valid no-op Function that
        // returns `undefined`. This also allows the callback itself to return
        // `undefined` to take the default result.
        else if (value.IsUndefined()) {
          handleResult(env, data->base * data->base, ResultLocation::DEFAULT);
        } else {
          handleError("Expected callback return to be of type number "
                      "| Promise<number>");
        }
      }

      // If no callback provided, handle with default result that the callback
      // would have provided.
      else {
        handleResult(env, data->base * data->base, ResultLocation::DEFAULT);
      }
    }
    // If `env` is nullptr, then all threads have called finished their usage of
    // the TSFN (either by calling `Release` or making a call and receiving
    // `napi_closing`). In this scenario, it is not allowed to call into
    // JavaScript, as the TSFN has been finalized.
    else {
      handleError("The TSFN has been finalized.");
    }
  }
}

// Full type of the ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<Context, DataType, CallJs>;
using base = tsfnutil::TSFNWrapBase<TSFNWrap, Context, TSFN>;

// A JS-accessible wrap that holds the TSFN.
class TSFNWrap : public base {
public:
  TSFNWrap(const CallbackInfo &info) : base(info) {}

  ~TSFNWrap() {
    for (auto &thread : finalizerData.threads) {
      // The TSFNWrap destructor runs when our ObjectWrap'd instance is
      // garbage-collected. This should never happen with proper usage of
      // `await` on `tsfn.release()`!
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  static std::array<ClassPropertyDescriptor<TSFNWrap>, 5> InstanceMethods() {
    return {{InstanceMethod("getContext", &TSFNWrap::GetContext),
             InstanceMethod("start", &TSFNWrap::Start),
             InstanceMethod("acquire", &TSFNWrap::Acquire),
             InstanceMethod("callCount", &TSFNWrap::CallCount),
             InstanceMethod("release", &TSFNWrap::Release)}};
  }

  bool logThread = DefaultOptions.logThread;
  bool logCall = DefaultOptions.logCall;
  bool hasEmptyCallback;
  std::atomic_uint succeededCalls;
  std::atomic_int aggregate;

  // The structure of the data send to the finalizer.
  struct FinalizerDataType {
    std::vector<std::thread> threads;
    std::vector<DataType> outstandingCalls;
    std::mutex
        callMutex; // To protect multi-threaded accesses to `outstandingCalls`
    std::unique_ptr<Promise::Deferred> deferred;
  } finalizerData;

  // Used for logging.
  std::mutex logMutex;

  Napi::Value Start(const CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (_tsfn) {
      NAPI_THROW(Napi::Error::New(Env(), "TSFN already exists."), Value());
    }

    // Creates a list to hold how many times each thread should make a call.
    std::vector<ThreadOptions> callCounts;

    // The JS-provided callback to execute for each call (if provided)
    Function callback;

    if (info.Length() > 0 && info[0].IsObject()) {
      auto arg0 = info[0].ToObject();

      if (arg0.Has("callback")) {
        auto cb = arg0.Get("callback");
        if (cb.IsUndefined()) {
          // An empty callback option will create a valid no-op function on
          // N-API 4 or leave `callback` as `std::nullptr` on N-API 5+.
          callback = TSFN::FunctionOrEmpty(env, callback);
        } else if (cb.IsFunction()) {
          callback = cb.As<Function>();
        } else {
          NAPI_THROW(Napi::TypeError::New(
                         Env(), "Invalid arguments: callback is not a "
                                "function. See StartOptions definition."),
                     Value());
        }
      }

      hasEmptyCallback = callback.IsEmpty();

      // Ensure proper parameters and add to our list of threads.
      if (arg0.Has("threads")) {
        Napi::Value threads = arg0.Get("threads");
        if (threads.IsArray()) {
          Napi::Array threadsArray = threads.As<Napi::Array>();
          for (auto i = 0U; i < threadsArray.Length(); ++i) {
            Napi::Value elem = threadsArray.Get(i);
            if (elem.IsObject()) {
              Object o = elem.ToObject();
              if (!(o.Has("calls") && o.Has("callDelay") &&
                    o.Has("threadDelay"))) {
                NAPI_THROW(Napi::TypeError::New(
                               Env(), "Invalid arguments. See "
                                      "StartOptions.threads definition."),
                           Value());
              }
              callCounts.push_back(ThreadOptions{
                  callCounts.size(), o.Get("calls").ToNumber(),
                  hasEmptyCallback ? -1 : o.Get("callDelay").ToNumber(),
                  o.Get("threadDelay").ToNumber()});
            } else {
              NAPI_THROW(Napi::TypeError::New(
                             Env(), "Invalid arguments. See "
                                    "StartOptions.threads definition."),
                         Value());
            }
          }
        } else {
          NAPI_THROW(
              Napi::TypeError::New(
                  Env(),
                  "Invalid arguments. See StartOptions.threads definition."),
              Value());
        }
      }

      if (arg0.Has("logCall")) {
        auto logCallOption = arg0.Get("logCall");
        if (logCallOption.IsBoolean()) {
          logCall = logCallOption.As<Boolean>();
        } else {
          NAPI_THROW(Napi::TypeError::New(
                         Env(), "Invalid arguments: logCall is not a boolean. "
                                "See StartOptions definition."),
                     Value());
        }
      }

      if (arg0.Has("logThread")) {
        auto logThreadOption = arg0.Get("logThread");
        if (logThreadOption.IsBoolean()) {
          logThread = logThreadOption.As<Boolean>();
        } else {
          NAPI_THROW(Napi::TypeError::New(
                         Env(), "Invalid arguments: logThread is not a "
                                "boolean. See StartOptions definition."),
                     Value());
        }
      }
    }

    const auto threadCount = callCounts.size();

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
        &finalizerData   // FinalizerDataType* data
    );

    if (logThread) {
      std::cout << "[Starting] Starting example with options: {\n[Starting]    "
                   "Log Call = "
                << (logCall ? "true" : "false")
                << ",\n[Starting]    Log Thread = "
                << (logThread ? "true" : "false")
                << ",\n[Starting]    Callback = "
                << (hasEmptyCallback ? "[empty]" : "function")
                << ",\n[Starting]    Threads = [\n";
      for (auto &threadOption : callCounts) {
        std::cout << "[Starting]        " << threadOption.threadId
                  << " -> { Calls: " << threadOption.calls << ", Call Delay: "
                  << (threadOption.callDelay == -1
                          ? "[no callback]"
                          : std::to_string(threadOption.callDelay))
                  << ", Thread Delay: " << threadOption.threadDelay << " },\n";
      }
      std::cout << "[Starting]    ]\n[Starting] }\n";
    }

    // for (auto threadId = 0U; threadId < threadCount; ++threadId) {
    for (auto &threadOption : callCounts) {
      finalizerData.threads.push_back(
          std::thread(threadEntry, _tsfn, threadOption, this));
    }

    return Number::New(env, threadCount);
  };

  Napi::Value Acquire(const CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (!_tsfn) {
      NAPI_THROW(Napi::Error::New(Env(), "TSFN does not exist."), Value());
    }

    // Creates a list to hold how many times each thread should make a call.
    std::vector<ThreadOptions> callCounts;
    if (info.Length() > 0 && info[0].IsArray()) {
      Napi::Array threadsArray = info[0].As<Napi::Array>();
      for (auto i = 0U; i < threadsArray.Length(); ++i) {
        Napi::Value elem = threadsArray.Get(i);
        if (elem.IsObject()) {
          Object o = elem.ToObject();
          if (!(o.Has("calls") && o.Has("callDelay") && o.Has("threadDelay"))) {
            NAPI_THROW(Napi::TypeError::New(Env(),
                                            "Invalid arguments. See "
                                            "StartOptions.threads definition."),
                       Value());
          }
          callCounts.push_back(ThreadOptions{
              callCounts.size() + finalizerData.threads.size(),
              o.Get("calls").ToNumber(),
              hasEmptyCallback ? -1 : o.Get("callDelay").ToNumber(),
              o.Get("threadDelay").ToNumber()});
        } else {
          NAPI_THROW(Napi::TypeError::New(Env(),
                                          "Invalid arguments. See "
                                          "StartOptions.threads definition."),
                     Value());
        }
      }
    } else {
      NAPI_THROW(
          Napi::TypeError::New(
              Env(), "Invalid arguments. See StartOptions.threads definition."),
          Value());
    }

    if (logThread) {
      for (auto &threadOption : callCounts) {
        std::cout << "[Acquire ]        " << threadOption.threadId
                  << " -> { Calls: " << threadOption.calls << ", Call Delay: "
                  << (threadOption.callDelay == -1
                          ? "[no callback]"
                          : std::to_string(threadOption.callDelay))
                  << ", Thread Delay: " << threadOption.threadDelay << " },\n";
      }
      std::cout << "[Acquire ]    ]\n[Acquire ] }\n";
    }

    auto started = 0U;

    for (auto &threadOption : callCounts) {
      // The `Acquire` call may be called from any thread, but we do it here to
      // avoid a race condition where the thread starts but the TSFN has been
      // finalized.
      auto status = _tsfn.Acquire();
      if (status == napi_ok) {
        finalizerData.threads.push_back(
            std::thread(threadEntry, _tsfn, threadOption, this));
        ++started;
      }
    }
    return Number::New(env, started);
  }


  // Release the TSFN from the Node thread. This will return a `Promise` that
  // resolves in the Finalizer.
  Napi::Value Release(const CallbackInfo &info) {
    if (finalizerData.deferred) {
      return finalizerData.deferred->Promise();
    }
    finalizerData.deferred.reset(
        new Promise::Deferred(Promise::Deferred::New(info.Env())));
    _tsfn.Release();
    return finalizerData.deferred->Promise();
  };

  // Returns an array corresponding to the amount of succeeded calls and the sum
  // aggregate.
  Napi::Value CallCount(const CallbackInfo &info) {
    Napi::Env env(info.Env());

    auto results = Array::New(env, 2);
    results.Set("0", Number::New(env, succeededCalls));
    results.Set("1", Number::New(env, aggregate));
    return results;
  };

  // Returns the TSFN's context.
  Napi::Value GetContext(const CallbackInfo &) {
    return _tsfn.GetContext()->Value();
  };

  // The thread entry point. It receives as arguments the TSFN, the call
  // options, and the context.
  static void threadEntry(TSFN tsfn, ThreadOptions options, Context *context) {
#define THREADLOG(X)                                                           \
  if (context->logThread) {                                                    \
    std::lock_guard<std::mutex> lock(context->logMutex);                       \
    std::cout << "[Thread " << threadId << "] [Native  ] "                     \
              << (data->callId == -1                                           \
                      ? ""                                                     \
                      : "[Call " + std::to_string(data->callId) + "] ")        \
              << X;                                                            \
  }

#define THREADLOG_MAIN(X)                                                      \
  if (context->logThread) {                                                    \
    std::lock_guard<std::mutex> lock(context->logMutex);                       \
    std::cout << "[Thread " << threadId << "] [Native  ] " << X;               \
  }
    using namespace std::chrono_literals;
    auto threadId = options.threadId;

    // To help with simultaneous threads using the logging mechanism, we'll
    // delay at thread start.
    std::this_thread::sleep_for(threadId * 10ms);
    THREADLOG_MAIN("Thread " << threadId << " started.\n")

    enum ThreadState {
      // Starting stating.
      Running,

      // When all requests have been completed.
      Release,

      // If a `NonBlockingCall` results in a Promise<number> but while waiting
      // for the resolution, the TSFN is finalized.
      AlreadyFinalized,

      // If a `NonBlockingCall` receiving `napi_closing`, we do *NOT* `Release`
      // it.
      Closing
    } state = ThreadState::Running;

    for (auto i = 0; state == Running; ++i) {

      if (i >= options.calls) {
        state = Release;
        break;
      }

      DataType data(context->makeNewCall(threadId, i, context->logCall,
                                         options.callDelay));

      if (options.threadDelay > 0 && i > 0) {
        THREADLOG("Delay for " << options.threadDelay
                               << "ms before next call\n")
        std::this_thread::sleep_for(options.threadDelay * 1ms);
      }

      THREADLOG("Performing call request: base = " << data->base << "\n")

      auto status = tsfn.NonBlockingCall(&data);

      if (status == napi_ok) {
        auto future = data->promise.get_future();
        auto result = future.get();
        if (result.error.length() == 0) {
          context->callSucceeded(data, result.result);
          THREADLOG("Receive answer: result = " << result.result << "\n")
          continue;
        } else if (result.isFinalized) {
          THREADLOG("Application Error: The TSFN has been finalized.\n")
          // If the Finalizer has canceled this request, we do not call
          // `Release()`.
          state = AlreadyFinalized;
        }
      } else if (status == napi_closing) {
        // A thread **MUST NOT** call `Abort()` or `Release()` if we receive an
        // `napi_closing` call.
        THREADLOG("N-API Error: The thread-safe function is aborted and "
                  "cannot accept more calls.\n")
        state = Closing;
      } else if (status == napi_queue_full) {
        // The example will finish this thread's use of the TSFN if it is full.
        THREADLOG("N-API Error: The queue was full when trying to call in a "
                  "non-blocking method.\n")
        state = Release;
      } else if (status == napi_invalid_arg) {
        THREADLOG("N-API Error: The thread-safe function is closed.\n")
        state = AlreadyFinalized;
      } else {
        THREADLOG("N-API Error: A generic error occurred when attemping to "
                  "add to the queue.\n")
        state = AlreadyFinalized;
      }
      context->callFailed(data);
    }

    THREADLOG_MAIN("Thread " << threadId << " finished. State: "
                             << (state == Closing ? "Closing"
                                                  : state == AlreadyFinalized
                                                        ? "Already Finalized"
                                                        : "Release")
                             << "\n")

    if (state == Release) {
      tsfn.Release();
    }
#undef THREADLOG
#undef THREADLOG_MAIN
  }

  // TSFN finalizer. Joins the threads and resolves the Promise returned by
  // `Release()` above.
  static void Finalizer(Napi::Env env, FinalizerDataType *finalizeDataPtr,
                        Context *ctx) {

    auto &finalizeData(*finalizeDataPtr);
    auto outstanding = finalizeData.outstandingCalls.size();
    if (ctx->logThread) {
      std::cout << "[Finalize] [Native  ] Joining threads (" << outstanding
                << " outstanding requests)...\n";
    }
    if (outstanding > 0) {
      for (auto &request : finalizeData.outstandingCalls) {
        request->promise.set_value(
            CallJsResult{-1, true, "The TSFN has been finalized."});
      }
    }
    for (auto &thread : finalizeData.threads) {
        thread.join();
    }

    ctx->clearTSFN();
    if (ctx->logThread) {
      std::cout << "[Finalize] [Native  ] Threads joined.\n";
    }

    finalizeData.deferred->Resolve(Boolean::New(env, true));
  }

  // This method does not run on the Node thread.
  void clearTSFN() { _tsfn = TSFN(); }

  // This method does not run on the Node thread.
  void callSucceeded(DataType data, int result) {
    std::lock_guard<std::mutex> lock(finalizerData.callMutex);
    succeededCalls++;
    aggregate += result;

    auto &calls = finalizerData.outstandingCalls;
    auto it = std::find_if(
        calls.begin(), calls.end(),
        [&](std::shared_ptr<Data> const &p) { return p.get() == data.get(); });

    if (it != calls.end()) {
      calls.erase(it);
    }
  }

  // This method does not run on the Node thread.
  void callFailed(DataType data) {
    std::lock_guard<std::mutex> lock(finalizerData.callMutex);
    auto &calls = finalizerData.outstandingCalls;
    auto it =
        std::find_if(calls.begin(), calls.end(),
                     [&](std::shared_ptr<Data> const &p) { return p == data; });

    if (it != calls.end()) {
      calls.erase(it);
    }
  }

  DataType makeNewCall(size_t threadId, int callId, bool logCall,
                       int callDelay) {
    // x
    // auto &calls(finalizerData.outstandingCalls);
    finalizerData.outstandingCalls.emplace_back(std::make_shared<Data>());
    auto data(finalizerData.outstandingCalls.back());
    data->threadId = threadId;
    data->logCall = logCall;
    data->callDelay = callDelay;
    data->base = threadId + 1;
    data->callId = callId;

    return data;
  }
};
} // namespace example

Object InitThreadSafeFunctionExExample(Env env) {
  auto exports(Object::New(env));
  example::TSFNWrap::Init(env, exports, "example");
  return exports;
}

#endif
