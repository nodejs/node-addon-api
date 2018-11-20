#include "napi.h"

using namespace Napi;

#if (NAPI_VERSION > 2)

class TestCleanupHook : public CleanupHook {
  public:
    static void DoCleanup(const CallbackInfo& info) {
	  static int secret = 42;
	  static int wrongSecret = 17;
	  TestCleanupHook* testCleanupHook = new TestCleanupHook(info, wrongSecret);
	  delete testCleanupHook;

      testCleanupHook = new TestCleanupHook(info, secret);
	}
	void Cleanup() {
	  printf("cleanup(%d)\n", _secret);
	};
  private:
    TestCleanupHook(const CallbackInfo& info, int secret) : CleanupHook(info.Env()), _secret(secret) {}
    int _secret;
};

Object InitCleanupHook(Env env) {
  Object exports = Object::New(env);
  exports["doCleanup"] = Function::New(env, TestCleanupHook::DoCleanup);
  return exports;
}

#endif
