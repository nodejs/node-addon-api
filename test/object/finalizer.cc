#include "napi.h"

using namespace Napi;

static int dummy;

Value AddFinalizer(const CallbackInfo& info) {
  ObjectReference* ref = new ObjectReference;
  *ref = Persistent(Object::New(info.Env()));
#ifdef NODE_API_EXPERIMENTAL_HAS_POST_FINALIZER
  info.Env().AddPostFinalizer(
#else
  info[0].As<Object>().AddFinalizer(
#endif
      [](Napi::Env /*env*/, ObjectReference* ref) {
        ref->Set("finalizerCalled", true);
        delete ref;
      },
      ref);
  return ref->Value();
}

Value AddFinalizerWithHint(const CallbackInfo& info) {
  ObjectReference* ref = new ObjectReference;
  *ref = Persistent(Object::New(info.Env()));
#ifdef NODE_API_EXPERIMENTAL_HAS_POST_FINALIZER
  info.Env().AddPostFinalizer(
#else
  info[0].As<Object>().AddFinalizer(
#endif
      [](Napi::Env /*env*/, ObjectReference* ref, int* dummy_p) {
        ref->Set("finalizerCalledWithCorrectHint", dummy_p == &dummy);
        delete ref;
      },
      ref,
      &dummy);
  return ref->Value();
}
