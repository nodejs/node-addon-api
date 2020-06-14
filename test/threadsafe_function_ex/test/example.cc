#include <array>
#include "napi.h"
#include "../util/util.h"

#if (NAPI_VERSION > 3)

using namespace Napi;

namespace example {

// Context of the TSFN.
using Context = Reference<Napi::Value>;

// Data passed (as pointer) to [Non]BlockingCall
using DataType = Promise::Deferred;

// CallJs callback function
static void CallJs(Napi::Env env, Napi::Function /*jsCallback*/,
                   Context *context, DataType *data) {
  if (env != nullptr) {
    if (data != nullptr) {
      data->Resolve(context->Value());
    }
  }
  if (data != nullptr) {
    delete data;
  }
}

// Full type of the ThreadSafeFunctionEx
using TSFN = ThreadSafeFunctionEx<Context, DataType, CallJs>;

class TSFNWrap;
using base = tsfnutil::TSFNWrapBase<TSFNWrap, Context, TSFN>;

// A JS-accessible wrap that holds the TSFN.
class TSFNWrap : public base {
public:
  TSFNWrap(const CallbackInfo &info) : base(info) {
    Napi::Env env = info.Env();

    Context *context = new Context(Persistent(info[0]));

    _tsfn = TSFN::New(
        env,                               // napi_env env,
        TSFN::DefaultFunctionFactory(env), // const Function& callback,
        Value(),                           // const Object& resource,
        "Test",                            // ResourceString resourceName,
        0,                                 // size_t maxQueueSize,
        1,                                 // size_t initialThreadCount,
        context,                           // Context* context,
        base::Finalizer,                   // Finalizer finalizer
        &_deferred                         // FinalizerDataType data
    );
  }

  static std::array<ClassPropertyDescriptor<TSFNWrap>, 3> InstanceMethods() {
    return {InstanceMethod("call", &TSFNWrap::Call),
            InstanceMethod("getContext", &TSFNWrap::GetContext),
            InstanceMethod("release", &TSFNWrap::Release)};
  }

  Napi::Value Call(const CallbackInfo &info) {
    auto *callData = new DataType(info.Env());
    _tsfn.NonBlockingCall(callData);
    return callData->Promise();
  };

  Napi::Value GetContext(const CallbackInfo &) {
    return _tsfn.GetContext()->Value();
  };
};
} // namespace context


Object InitThreadSafeFunctionExExample(Env env) {
  auto exports(Object::New(env));
  example::TSFNWrap::Init(env, exports, "example");
  return exports;
}


#endif
