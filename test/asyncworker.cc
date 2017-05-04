#include "napi.h"

using namespace Napi;

class TestWorker : public AsyncWorker {
public:
  static void DoWork(const CallbackInfo& info) {
    bool succeed = info[0].As<Boolean>();
    Function cb = info[1].As<Function>();
    Value data = info[2];

    TestWorker* worker = new TestWorker(cb);
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
  TestWorker(Function cb) : AsyncWorker(cb) {}
  bool _succeed;
};

Object InitAsyncWorker(Env env) {
  Object exports = Object::New(env);
  exports["doWork"] = Function::New(env, TestWorker::DoWork);
  return exports;
}
