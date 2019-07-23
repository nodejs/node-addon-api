#include <chrono>
#include <thread>
#include "napi.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

constexpr size_t ARRAY_LENGTH = 10;
constexpr size_t MAX_QUEUE_SIZE = 2;

static std::thread threads[2];
static ThreadSafeFunction tsfn;

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
} tsfnInfo;

// Thread data to transmit to JS
static int ints[ARRAY_LENGTH];

static void SecondaryThread() {
  if (tsfn.Release() != napi_ok) {
    Error::Fatal("SecondaryThread", "ThreadSafeFunction.Release() failed");
  }
}

// Source thread producing the data
static void DataSourceThread() {
  ThreadSafeFunctionInfo* info = tsfn.GetContext();

  if (info->startSecondary) {
    if (tsfn.Acquire() != napi_ok) {
      Error::Fatal("DataSourceThread", "ThreadSafeFunction.Acquire() failed");
    }

    threads[1] = std::thread(SecondaryThread);
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
        status = tsfn.BlockingCall();
        break;
      case ThreadSafeFunctionInfo::BLOCKING:
        status = tsfn.BlockingCall(&ints[index], callback);
        break;
      case ThreadSafeFunctionInfo::NON_BLOCKING:
        status = tsfn.NonBlockingCall(&ints[index], callback);
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

  if (!queueWasClosing && tsfn.Release() != napi_ok) {
    Error::Fatal("DataSourceThread", "ThreadSafeFunction.Release() failed");
  }
}

static Value StopThread(const CallbackInfo& info) {
  tsfnInfo.jsFinalizeCallback = Napi::Persistent(info[0].As<Function>());
  bool abort = info[1].As<Boolean>();
  if (abort) {
    tsfn.Abort();
  } else {
    tsfn.Release();
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

static Value StartThreadInternal(const CallbackInfo& info,
    ThreadSafeFunctionInfo::CallType type) {
  tsfnInfo.type = type;
  tsfnInfo.abort = info[1].As<Boolean>();
  tsfnInfo.startSecondary = info[2].As<Boolean>();
  tsfnInfo.maxQueueSize = info[3].As<Number>().Uint32Value();

  tsfn = ThreadSafeFunction::New(info.Env(), info[0].As<Function>(),
      "Test", tsfnInfo.maxQueueSize, 2, &tsfnInfo, JoinTheThreads, threads);

  threads[0] = std::thread(DataSourceThread);

  return Value();
}

static Value Release(const CallbackInfo& /* info */) {
  if (tsfn.Release() != napi_ok) {
    Error::Fatal("Release", "ThreadSafeFunction.Release() failed");
  }
  return Value();
}

static Value StartThread(const CallbackInfo& info) {
  return StartThreadInternal(info, ThreadSafeFunctionInfo::BLOCKING);
}

static Value StartThreadNonblocking(const CallbackInfo& info) {
  return StartThreadInternal(info, ThreadSafeFunctionInfo::NON_BLOCKING);
}

static Value StartThreadNoNative(const CallbackInfo& info) {
  return StartThreadInternal(info, ThreadSafeFunctionInfo::DEFAULT);
}

Object InitThreadSafeFunction(Env env) {
  for (size_t index = 0; index < ARRAY_LENGTH; index++) {
    ints[index] = index;
  }

  Object exports = Object::New(env);
  exports["ARRAY_LENGTH"] = Number::New(env, ARRAY_LENGTH);
  exports["MAX_QUEUE_SIZE"] = Number::New(env, MAX_QUEUE_SIZE);
  exports["startThread"] = Function::New(env, StartThread);
  exports["startThreadNoNative"] = Function::New(env, StartThreadNoNative);
  exports["startThreadNonblocking"] =
      Function::New(env, StartThreadNonblocking);
  exports["stopThread"] = Function::New(env, StopThread);
  exports["release"] = Function::New(env, Release);

  return exports;
}

#endif
