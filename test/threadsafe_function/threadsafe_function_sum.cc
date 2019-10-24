#include "napi.h"
#include <thread>
#include <future>

#if (NAPI_VERSION > 3)

using namespace Napi;
using namespace std;

namespace {

struct TestData {

  TestData(Promise::Deferred&& deferred) : deferred(std::move(deferred)) {};
  
  // Native Promise returned to JavaScript
  Promise::Deferred deferred;

  // List of threads created for test. This list only ever accessed via main
  // thread.
  vector<thread> threads = {};

  ThreadSafeFunction tsfn = ThreadSafeFunction();
};

void FinalizerCallback(Napi::Env env, TestData* finalizeData){
  for (size_t i = 0; i < finalizeData->threads.size(); ++i) {
    finalizeData->threads[i].join();
  }
  finalizeData->deferred.Resolve(Boolean::New(env,true));
  delete finalizeData;
}

/**
 * See threadsafe_function_sum.js for descriptions of the tests in this file
 */

void entryWithTSFN(ThreadSafeFunction tsfn, int threadId) {
  std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 100 + 1));
  tsfn.BlockingCall( [=](Napi::Env env, Function callback) {
    callback.Call( { Number::New(env, static_cast<double>(threadId))});
  });
  tsfn.Release();
}

static Value TestWithTSFN(const CallbackInfo& info) {
  int threadCount = info[0].As<Number>().Int32Value();
  Function cb = info[1].As<Function>();

  // We pass the test data to the Finalizer for cleanup. The finalizer is
  // responsible for deleting this data as well.
  TestData *testData = new TestData(Promise::Deferred::New(info.Env()));

  ThreadSafeFunction tsfn = ThreadSafeFunction::New(
      info.Env(), cb, "Test", 0, threadCount,
      std::function<decltype(FinalizerCallback)>(FinalizerCallback), testData);

  for (int i = 0; i < threadCount; ++i) {
    // A copy of the ThreadSafeFunction will go to the thread entry point
    testData->threads.push_back( thread(entryWithTSFN, tsfn, i) );
  }

  return testData->deferred.Promise();
}


void entryDelayedTSFN(std::future<ThreadSafeFunction> tsfnFuture, int threadId) {
  std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 100 + 1));
  ThreadSafeFunction tsfn = tsfnFuture.get();
  tsfn.BlockingCall( [=](Napi::Env env, Function callback) {
    callback.Call( { Number::New(env, static_cast<double>(threadId))});
  });
  tsfn.Release();
}

static Value TestDelayedTSFN(const CallbackInfo& info) {
  int threadCount = info[0].As<Number>().Int32Value();
  Function cb = info[1].As<Function>();

  TestData *testData = new TestData(Promise::Deferred::New(info.Env()));

  vector< std::promise<ThreadSafeFunction> > tsfnPromises;

  for (int i = 0; i < threadCount; ++i) {
    tsfnPromises.emplace_back();
    testData->threads.push_back( thread(entryDelayedTSFN, tsfnPromises[i].get_future(), i) );
  }

  testData->tsfn = ThreadSafeFunction::New(
      info.Env(), cb, "Test", 0, threadCount,
      std::function<decltype(FinalizerCallback)>(FinalizerCallback), testData);

  for (int i = 0; i < threadCount; ++i) {
    tsfnPromises[i].set_value(testData->tsfn);
  }

  return testData->deferred.Promise();
}

void entryAcquire(ThreadSafeFunction tsfn, int threadId) {
  tsfn.Acquire();
  std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 100 + 1));
  tsfn.BlockingCall( [=](Napi::Env env, Function callback) {
    callback.Call( { Number::New(env, static_cast<double>(threadId))});
  });
  tsfn.Release();
}

static Value CreateThread(const CallbackInfo& info) {
  TestData* testData = static_cast<TestData*>(info.Data());
  ThreadSafeFunction tsfn = testData->tsfn;
  int threadId = testData->threads.size();
  // A copy of the ThreadSafeFunction will go to the thread entry point
  testData->threads.push_back( thread(entryAcquire, tsfn, threadId) );
  return Number::New(info.Env(), threadId);
}

static Value StopThreads(const CallbackInfo& info) {
  TestData* testData = static_cast<TestData*>(info.Data());
  ThreadSafeFunction tsfn = testData->tsfn;
  tsfn.Release();
  return info.Env().Undefined();
}

static Value TestAcquire(const CallbackInfo& info) {
  Function cb = info[0].As<Function>();
  Napi::Env env = info.Env();

  // We pass the test data to the Finalizer for cleanup. The finalizer is
  // responsible for deleting this data as well.
  TestData *testData = new TestData(Promise::Deferred::New(info.Env()));

  testData->tsfn = ThreadSafeFunction::New(
      env, cb, "Test", 0, 1,
      std::function<decltype(FinalizerCallback)>(FinalizerCallback), testData);

  Object result = Object::New(env);
  result["createThread"] = Function::New( env, CreateThread, "createThread", testData);
  result["stopThreads"] = Function::New( env, StopThreads, "stopThreads", testData);
  result["promise"] = testData->deferred.Promise();

  return result;
}
}

Object InitThreadSafeFunctionSum(Env env) {
  Object exports = Object::New(env);
  exports["testDelayedTSFN"] = Function::New(env, TestDelayedTSFN);
  exports["testWithTSFN"] = Function::New(env, TestWithTSFN);
  exports["testAcquire"] = Function::New(env, TestAcquire);
  return exports;
}

#endif
