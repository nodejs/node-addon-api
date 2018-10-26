#include "napi.h"

using namespace Napi;

class TestPromise : public AsyncPromise::BaseTask {
public:
  enum class Test
  {
    succeed_before_queue = 1,
    succeed_in_execute,
    fail_before_queue,
    fail_in_execute
  };

  static Value MakePromise(const CallbackInfo& info) {
    return AsyncPromise::New<TestPromise>(info).Promise();
  }

protected:
  void Execute() override {
    if (!_succeed) {
      SetError(_data);
    }
  }

  void OnInit(const CallbackInfo& info) override {
    auto test = (Test)info[0].As<Number>().Uint32Value();
    Object resource = info[1].As<Object>();
    _data = info[2].As<String>();

    switch (test)
    {
    case Test::succeed_before_queue:
      _succeed = true;
      Resolve(String::New(info.Env(), _data));
      break;

    case Test::succeed_in_execute:
      _succeed = true;
      break;

      case Test::fail_before_queue:
      _succeed = false;
      Reject(Error::New(info.Env(), _data).Value());
      break;

      case Test::fail_in_execute:
      _succeed = false;
      break;
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
