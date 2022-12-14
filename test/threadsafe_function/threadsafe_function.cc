#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "napi.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

// Array length of 10
constexpr size_t ARRAY_LENGTH = 10;

// The queue can at most be 2
constexpr size_t MAX_QUEUE_SIZE = 2;

// Two threads
static std::thread threads[2];
static ThreadSafeFunction s_tsfn;

// Metadata that describes the threadsafe function at hand
// Saved as the threadsafe function context
struct ThreadSafeFunctionInfo {
  enum CallType {
    DEFAULT,
    BLOCKING,
    NON_BLOCKING,
    NON_BLOCKING_DEFAULT,
    NON_BLOCKING_SINGLE_ARG
  } type;
  bool abort;
  bool startSecondary;
  FunctionReference jsFinalizeCallback;
  uint32_t maxQueueSize;
  bool closeCalledFromJs;
  std::mutex protect;
  std::condition_variable signal;
} tsfnInfo;

// Thread data to transmit to JS
static int ints[ARRAY_LENGTH];

// "Secondary thread"
static void SecondaryThread() {
  if (s_tsfn.Release() != napi_ok) {
    Error::Fatal("SecondaryThread", "ThreadSafeFunction.Release() failed");
  }
}

// The thread that is producing data
// Source thread producing the data
static void DataSourceThread() {
  ThreadSafeFunctionInfo* info = s_tsfn.GetContext();

  // If we need to spawn a secondary thread from the main thread
  if (info->startSecondary) {
    if (s_tsfn.Acquire() != napi_ok) {
      Error::Fatal("DataSourceThread", "ThreadSafeFunction.Acquire() failed");
    }
    // otherwise, spawn the secondary thread
    threads[1] = std::thread(SecondaryThread);
  }

  bool queueWasFull = false;
  bool queueWasClosing = false;

  // for loop condition:
  //   Index starts at the last idx of the array,
  //   AND the queue wasn't closing, decreament index and keep going
  for (int index = ARRAY_LENGTH - 1; index > -1 && !queueWasClosing; index--) {
    // Set status as generic failure
    napi_status status = napi_generic_failure;

    // Generic callback function
    auto callback = [](Env env, Function jsCallback, int* data) {
      // Calling js with the data
      jsCallback.Call({Number::New(env, *data)});
    };

    auto noArgCallback = [](Env env, Function jsCallback) {
      jsCallback.Call({Number::New(env, 42)});
    };
    // Swtich base on types
    switch (info->type) {
      case ThreadSafeFunctionInfo::DEFAULT:
        status = s_tsfn.BlockingCall();
        break;
      case ThreadSafeFunctionInfo::BLOCKING:
        status = s_tsfn.BlockingCall(&ints[index], callback);
        break;
      case ThreadSafeFunctionInfo::NON_BLOCKING:
        status = s_tsfn.NonBlockingCall(&ints[index], callback);
        break;
      case ThreadSafeFunctionInfo::NON_BLOCKING_DEFAULT:
        status = s_tsfn.NonBlockingCall();
        break;

      case ThreadSafeFunctionInfo::NON_BLOCKING_SINGLE_ARG:
        status = s_tsfn.NonBlockingCall(noArgCallback);
        break;
    }

    if (info->abort && (info->type == ThreadSafeFunctionInfo::BLOCKING ||
                        info->type == ThreadSafeFunctionInfo::DEFAULT)) {
      // Let's make this thread really busy to give the main thread a chance to
      // abort / close.
      std::unique_lock<std::mutex> lk(info->protect);
      while (!info->closeCalledFromJs) {
        info->signal.wait(lk);
      }
    }

    switch (status) {
      case napi_queue_full:
        queueWasFull = true;
        index++;
        // fall through

      case napi_ok:
        continue;

      case napi_closing:
        queueWasClosing = true;
        break;

      default:
        Error::Fatal("DataSourceThread", "ThreadSafeFunction.*Call() failed");
    }
  }

  if (info->type == ThreadSafeFunctionInfo::NON_BLOCKING && !queueWasFull) {
    Error::Fatal("DataSourceThread", "Queue was never full");
  }

  if (info->abort && !queueWasClosing) {
    Error::Fatal("DataSourceThread", "Queue was never closing");
  }

  if (!queueWasClosing && s_tsfn.Release() != napi_ok) {
    Error::Fatal("DataSourceThread", "ThreadSafeFunction.Release() failed");
  }
}

// Stops the thread from js
static Value StopThread(const CallbackInfo& info) {
  tsfnInfo.jsFinalizeCallback = Napi::Persistent(info[0].As<Function>());
  bool abort = info[1].As<Boolean>();
  if (abort) {
    s_tsfn.Abort();
  } else {
    s_tsfn.Release();
  }
  {
    std::lock_guard<std::mutex> _(tsfnInfo.protect);
    tsfnInfo.closeCalledFromJs = true;
    tsfnInfo.signal.notify_one();
  }
  return Value();
}

// Join the thread and inform JS that we're done.
static void JoinTheThreads(Env /* env */,
                           std::thread* theThreads,
                           ThreadSafeFunctionInfo* info) {
  theThreads[0].join();
  if (info->startSecondary) {
    theThreads[1].join();
  }

  info->jsFinalizeCallback.Call({});
  info->jsFinalizeCallback.Reset();
}

// The function that does the heavy liftin
static Value StartThreadInternal(const CallbackInfo& info,
                                 ThreadSafeFunctionInfo::CallType type) {
  tsfnInfo.type = type;
  tsfnInfo.abort = info[1].As<Boolean>();
  tsfnInfo.startSecondary = info[2].As<Boolean>();
  tsfnInfo.maxQueueSize = info[3].As<Number>().Uint32Value();
  tsfnInfo.closeCalledFromJs = false;

  s_tsfn = ThreadSafeFunction::New(info.Env(),
                                   info[0].As<Function>(),
                                   "Test",
                                   tsfnInfo.maxQueueSize,
                                   2,
                                   &tsfnInfo,
                                   JoinTheThreads,
                                   threads);

  threads[0] = std::thread(DataSourceThread);

  return Value();
}

static Value Release(const CallbackInfo& /* info */) {
  if (s_tsfn.Release() != napi_ok) {
    Error::Fatal("Release", "ThreadSafeFunction.Release() failed");
  }
  return Value();
}

// Entry point for starting thread, in blocking mode
static Value StartThread(const CallbackInfo& info) {
  return StartThreadInternal(info, ThreadSafeFunctionInfo::BLOCKING);
}

// Entry point for starting thread, in nonblocking mode
static Value StartThreadNonblocking(const CallbackInfo& info) {
  return StartThreadInternal(info, ThreadSafeFunctionInfo::NON_BLOCKING);
}

// Entry point for starting thread, in block, no args
static Value StartThreadNoNative(const CallbackInfo& info) {
  return StartThreadInternal(info, ThreadSafeFunctionInfo::DEFAULT);
}

static Value StartThreadNonblockingNoNative(const CallbackInfo& info) {
  return StartThreadInternal(info,
                             ThreadSafeFunctionInfo::NON_BLOCKING_DEFAULT);
}

static Value StartThreadNonBlockingSingleArg(const CallbackInfo& info) {
  return StartThreadInternal(info,
                             ThreadSafeFunctionInfo::NON_BLOCKING_SINGLE_ARG);
}

// Entry point for the addon
Object InitThreadSafeFunction(Env env) {
  for (size_t index = 0; index < ARRAY_LENGTH; index++) {
    ints[index] = index;
  }

  Object exports = Object::New(env);
  exports["ARRAY_LENGTH"] = Number::New(env, ARRAY_LENGTH);
  exports["MAX_QUEUE_SIZE"] = Number::New(env, MAX_QUEUE_SIZE);
  exports["startThread"] = Function::New(env, StartThread);
  exports["startThreadNoNative"] = Function::New(env, StartThreadNoNative);
  exports["startThreadNonblockingNoNative"] =
      Function::New(env, StartThreadNonblockingNoNative);
  exports["startThreadNonblocking"] =
      Function::New(env, StartThreadNonblocking);
  exports["startThreadNonblockSingleArg"] =
      Function::New(env, StartThreadNonBlockingSingleArg);
  exports["stopThread"] = Function::New(env, StopThread);
  exports["release"] = Function::New(env, Release);

  return exports;
}

#endif
