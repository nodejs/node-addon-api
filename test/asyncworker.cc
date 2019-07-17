#include "napi.h"

using namespace Napi;

class TestWorker : public AsyncWorker {
public:
  static void DoWork(const CallbackInfo& info) {
    bool succeed = info[0].As<Boolean>();
    Object resource = info[1].As<Object>();
    Function cb = info[2].As<Function>();
    Value data = info[3];

    TestWorker* worker = new TestWorker(cb, "TestResource", resource);
    worker->Receiver().Set("data", data);
    worker->_succeed = succeed;
    worker->Queue();
  }

protected:
  void Execute() override {
    if (!_succeed) {
      SetError("test error");
    }
  }

private:
  TestWorker(Function cb, const char* resource_name, const Object& resource)
      : AsyncWorker(cb, resource_name, resource) {}
  bool _succeed;
};

class TestWorkerWithResult : public AsyncWorker {
public:
  static void DoWork(const CallbackInfo& info) {
    bool succeed = info[0].As<Boolean>();
    Object resource = info[1].As<Object>();
    Function cb = info[2].As<Function>();
    Value data = info[3];

    TestWorkerWithResult* worker = new TestWorkerWithResult(cb, "TestResource", resource);
    worker->Receiver().Set("data", data);
    worker->_succeed = succeed;
    worker->Queue();
  }

protected:
  void Execute() override {
    if (!_succeed) {
      SetError("test error");
    }
  }

  std::vector<napi_value> GetResult(Napi::Env env) override {
    return {Boolean::New(env, _succeed),
            String::New(env, _succeed ? "ok" : "error")};
  }

private:
  TestWorkerWithResult(Function cb, const char* resource_name, const Object& resource)
      : AsyncWorker(cb, resource_name, resource) {}
  bool _succeed;
};

class TestWorkerNoCallback : public AsyncWorker {
public:
  static Value DoWork(const CallbackInfo& info) {
    napi_env env = info.Env();
    bool succeed = info[0].As<Boolean>();
    Object resource = info[1].As<Object>();

    TestWorkerNoCallback* worker = new TestWorkerNoCallback(env, "TestResource", resource);
    worker->_succeed = succeed;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override {
  }
  virtual void OnOK() override {
      _deferred.Resolve(Env().Undefined());

  }
  virtual void OnError(const Napi::Error& /* e */) override {
      _deferred.Reject(Env().Undefined());
  }

private:
  TestWorkerNoCallback(napi_env env, const char* resource_name, const Object& resource)
      : AsyncWorker(env, resource_name, resource), _deferred(Napi::Promise::Deferred::New(env)) {
      }
  Promise::Deferred _deferred;
  bool _succeed;
};

Object InitAsyncWorker(Env env) {
  Object exports = Object::New(env);
  exports["doWork"] = Function::New(env, TestWorker::DoWork);
  exports["doWorkNoCallback"] = Function::New(env, TestWorkerNoCallback::DoWork);
  exports["doWorkWithResult"] = Function::New(env, TestWorkerWithResult::DoWork);
  return exports;
}
