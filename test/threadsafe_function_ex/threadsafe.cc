#include <chrono>
#include <thread>
#include "napi.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

constexpr size_t ARRAY_LENGTH = 10;
constexpr size_t MAX_QUEUE_SIZE = 2;

static std::thread threadsEx[2];
static ThreadSafeFunction tsfnEx;

struct ThreadSafeFunctionInfo {
  enum CallType {
    DEFAULT,
    BLOCKING,
    NON_BLOCKING
  } type;
  bool abort;
  bool startSecondary;
  FunctionReference jsFinalizeCallback;
  uint32_t maxQueueSize;
} tsfnInfoEx;

// Thread data to transmit to JS
static int intsEx[ARRAY_LENGTH];

static void SecondaryThreadEx() {
  if (tsfnEx.Release() != napi_ok) {
    Error::Fatal("SecondaryThread", "ThreadSafeFunction.Release() failed");
  }
}

// Source thread producing the data
static void DataSourceThreadEx() {
  ThreadSafeFunctionInfo* info = tsfnEx.GetContext();

  if (info->startSecondary) {
    if (tsfnEx.Acquire() != napi_ok) {
      Error::Fatal("DataSourceThread", "ThreadSafeFunction.Acquire() failed");
    }

    threadsEx[1] = std::thread(SecondaryThreadEx);
  }

  bool queueWasFull = false;
  bool queueWasClosing = false;
  for (int index = ARRAY_LENGTH - 1; index > -1 && !queueWasClosing; index--) {
    napi_status status = napi_generic_failure;
    auto callback = [](Env env, Function jsCallback, int* data) {
      jsCallback.Call({ Number::New(env, *data) });
    };

    switch (info->type) {
      case ThreadSafeFunctionInfo::DEFAULT:
        status = tsfnEx.BlockingCall();
        break;
      case ThreadSafeFunctionInfo::BLOCKING:
        status = tsfnEx.BlockingCall(&intsEx[index], callback);
        break;
      case ThreadSafeFunctionInfo::NON_BLOCKING:
        status = tsfnEx.NonBlockingCall(&intsEx[index], callback);
        break;
    }

    if (info->maxQueueSize == 0) {
      // Let's make this thread really busy for 200 ms to give the main thread a
      // chance to abort.
      auto start = std::chrono::high_resolution_clock::now();
      constexpr auto MS_200 = std::chrono::milliseconds(200);
      for (; std::chrono::high_resolution_clock::now() - start < MS_200;);
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

  if (!queueWasClosing && tsfnEx.Release() != napi_ok) {
    Error::Fatal("DataSourceThread", "ThreadSafeFunction.Release() failed");
  }
}

static Value StopThreadEx(const CallbackInfo& info) {
  tsfnInfoEx.jsFinalizeCallback = Napi::Persistent(info[0].As<Function>());
  bool abort = info[1].As<Boolean>();
  if (abort) {
    tsfnEx.Abort();
  } else {
    tsfnEx.Release();
  }
  return Value();
}

// Join the thread and inform JS that we're done.
static void JoinTheThreadsEx(Env /* env */,
                           std::thread* theThreads,
                           ThreadSafeFunctionInfo* info) {
  theThreads[0].join();
  if (info->startSecondary) {
    theThreads[1].join();
  }

  info->jsFinalizeCallback.Call({});
  info->jsFinalizeCallback.Reset();
}

static Value StartThreadInternalEx(const CallbackInfo& info,
    ThreadSafeFunctionInfo::CallType type) {
  tsfnInfoEx.type = type;
  tsfnInfoEx.abort = info[1].As<Boolean>();
  tsfnInfoEx.startSecondary = info[2].As<Boolean>();
  tsfnInfoEx.maxQueueSize = info[3].As<Number>().Uint32Value();

  tsfnEx = ThreadSafeFunction::New(info.Env(), info[0].As<Function>(),
      "Test", tsfnInfoEx.maxQueueSize, 2, &tsfnInfoEx, JoinTheThreadsEx, threadsEx);

  threadsEx[0] = std::thread(DataSourceThreadEx);

  return Value();
}

static Value ReleaseEx(const CallbackInfo& /* info */) {
  if (tsfnEx.Release() != napi_ok) {
    Error::Fatal("Release", "ThreadSafeFunction.Release() failed");
  }
  return Value();
}

static Value StartThreadEx(const CallbackInfo& info) {
  return StartThreadInternalEx(info, ThreadSafeFunctionInfo::BLOCKING);
}

static Value StartThreadNonblockingEx(const CallbackInfo& info) {
  return StartThreadInternalEx(info, ThreadSafeFunctionInfo::NON_BLOCKING);
}

static Value StartThreadNoNativeEx(const CallbackInfo& info) {
  return StartThreadInternalEx(info, ThreadSafeFunctionInfo::DEFAULT);
}

Object InitThreadSafeFunctionExThreadSafe(Env env) {
  for (size_t index = 0; index < ARRAY_LENGTH; index++) {
    intsEx[index] = index;
  }

  Object exports = Object::New(env);
  exports["ARRAY_LENGTH"] = Number::New(env, ARRAY_LENGTH);
  exports["MAX_QUEUE_SIZE"] = Number::New(env, MAX_QUEUE_SIZE);
  exports["startThread"] = Function::New(env, StartThreadEx);
  exports["startThreadNoNative"] = Function::New(env, StartThreadNoNativeEx);
  exports["startThreadNonblocking"] =
      Function::New(env, StartThreadNonblockingEx);
  exports["stopThread"] = Function::New(env, StopThreadEx);
  exports["release"] = Function::New(env, ReleaseEx);

  return exports;
}

#endif
