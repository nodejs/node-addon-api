#include "napi.h"

using namespace Napi;

class TestPromise : public AsyncPromise<TestPromise>::BaseTask {
public:
  enum class Test
  {
    succeed_before_queue = 1,
    succeed_in_execute,
    fail_before_queue,
    fail_in_execute
  };

  static Value MakePromise(const CallbackInfo& info) {
    auto test = (Test)info[0].As<Number>().Uint32Value();
    Object resource = info[1].As<Object>();
    std::string data = info[2].As<String>();

    AsyncPromise<TestPromise> async_promise(info.Env(), "TestResource", resource);

    async_promise.Task()->_data = data;

    switch (test)
    {
    case Test::succeed_before_queue:
      async_promise.Resolve(String::New(info.Env(), data));
      break;

      case Test::succeed_in_execute:
      async_promise.Task()->_succeed = true;
      async_promise.Queue();
      break;

      case Test::fail_before_queue:
      async_promise.Reject(Error::New(info.Env(), data).Value());
      break;

      case Test::fail_in_execute:
      async_promise.Task()->_succeed = false;
      async_promise.Queue();
      break;
    }

    return async_promise.Promise();
  }

  TestPromise(Napi::Env env)
      : Base_t(env) {}

protected:
  void Execute() override {
    if (!_succeed) {
      SetError(_data);
    }
  }

private:
  bool _succeed;
  std::string _data;
};

Object InitAsyncPromise(Env env) {
  Object exports = Object::New(env);
  exports["makePromise"] = Function::New(env, TestPromise::MakePromise);
  return exports;
}
