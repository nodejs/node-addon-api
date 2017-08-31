#include <napi.h>

class Test : public Napi::ObjectWrap<Test>
{
public:
    Test(const Napi::CallbackInfo& info) :
        Napi::ObjectWrap<Test>(info)
    {
    }

    void Set(const Napi::CallbackInfo& info)
    {
        value = info[0].As<Napi::Number>();
    }

    Napi::Value Get(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), value);
    }

    static void Initialize(Napi::Env env, Napi::Object exports)
    {
        exports.Set("Test", DefineClass(env, "Test",
        {
            InstanceMethod("test_set", &Test::Set),
            InstanceMethod("test_get", &Test::Get)
        }));
    }

private:
    uint32_t value;
};

Napi::Object InitObjectWrap(Napi::Env env)
{
    Napi::Object exports = Napi::Object::New(env);
    Test::Initialize(env, exports);
    return exports;
}

