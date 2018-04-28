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

Object InitAsyncWorker(Env env) {
  Object exports = Object::New(env);
  exports["doWork"] = Function::New(env, TestWorker::DoWork);
  return exports;
}
