#include <napi.h>
#include <iostream>
using namespace Napi;

class NewMockWorker : public AsyncWorker {
 public:
  NewMockWorker(const Function& cb) : AsyncWorker(cb) {}

 protected:
  void Execute() override {}
};

void RunAsyncWorkerUnitTests(const CallbackInfo& info) {
  Function f = Function(info.Env(), Object::New(info.Env()));
  NewMockWorker newMock(f);

  AsyncWorkerUnitTest unitTest(newMock);

  // TODO: Add other methods to get receiver cb, etc
  std::cout << "Result  " << unitTest.isSuppressDest() << std::endl;
}

Napi::Object InitAsyncWorkerUnitTest(Napi::Env env) {
  Object exports = Object::New(env);
  exports["runAsyncWorkerUnitTests"] =
      Function::New(env, RunAsyncWorkerUnitTests);
  return exports;
}
