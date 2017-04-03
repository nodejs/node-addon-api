#ifndef SRC_NAPI_H_
#define SRC_NAPI_H_

////////////////////////////////////////////////////////////////////////////////
// NAPI C++ Wrapper Classes
//
// These classes wrap the "NAPI" ABI-stable C APIs for Node.js, providing a
// C++ object model and C++ exception-handling semantics with low overhead.
// The wrappers are all header-only so that they do not affect the ABI.
////////////////////////////////////////////////////////////////////////////////

#include "node_api.h"
#include <functional>
#include <initializer_list>
#include <string>
#include <vector>

namespace Napi {

  class Env;
  class Value;
  class Boolean;
  class Number;
  class String;
  class Object;
  class Array;
  class Function;
  template <typename T> class Buffer;
  class Error;
  class PropertyDescriptor;
  class CallbackInfo;
  template <typename T> class Reference;

  class TypedArray;
  template <typename T, napi_typedarray_type A> class TypedArray_;
  typedef TypedArray_<int8_t, napi_int8> Int8Array;
  typedef TypedArray_<uint8_t, napi_uint8> Uint8Array;
  typedef TypedArray_<uint8_t, napi_uint8_clamped> Uint8ClampedArray;
  typedef TypedArray_<int16_t, napi_int16> Int16Array;
  typedef TypedArray_<uint16_t, napi_uint16> Uint16Array;
  typedef TypedArray_<int32_t, napi_int32> Int32Array;
  typedef TypedArray_<uint32_t, napi_uint32> Uint32Array;
  typedef TypedArray_<float, napi_float32> Float32Array;
  typedef TypedArray_<double, napi_float64> Float64Array;

  // Functions exposed to JavaScript must conform to one these callback signatures.
  // These are std::function<> typedefs instead of function pointers to enable lambdas.
  // (See ObjectWrap<T> for callbacks used when wrapping entire classes.)
  typedef std::function<void(const CallbackInfo& info)> VoidFunctionCallback;
  typedef std::function<Value(const CallbackInfo& info)> FunctionCallback;

  // A N-API C++ module's registration callback (init) function has this sinature.
  typedef void ModuleRegisterCallback(Env env, Object exports, Object module);

  /*
   * Environment for NAPI operations. (In V8 this corresponds to an Isolate.)
   */
  class Env {
  public:
    Env(napi_env env);

    operator napi_env() const;

    Object Global() const;
    Value Undefined() const;
    Value Null() const;

    bool IsExceptionPending() const;

  private:
    napi_env _env;
  };

  /*
   * Represents a JavaScript value of unknown type.
   *
   * For type-specific operations, convert to one of the Value subclasses using a
   * To* or As* method. The To* methods do type coercion; the As* methods do not.
   */
  class Value {
  public:
    Value();
    Value(napi_env env, napi_value value);

    operator napi_value() const;
    bool operator ==(const Value& other) const;
    bool operator !=(const Value& other) const;
    bool StrictEquals(const Value& other) const;

    Env Env() const;

    napi_valuetype Type() const;
    bool IsUndefined() const;
    bool IsNull() const;
    bool IsBoolean() const;
    bool IsNumber() const;
    bool IsString() const;
    bool IsSymbol() const;
    bool IsArray() const;
    bool IsArrayBuffer() const;
    bool IsTypedArray() const;
    bool IsObject() const;
    bool IsFunction() const;
    bool IsBuffer() const;

    // Use As<T> to convert to another type of Napi::Value, when the actual type is
    // known or assumed. This conversion does NOT very the type. But calling any methods
    // inappropriate for the actual value type will cause Napi::Errors to be thrown.
    template <typename T> T As() const;

    Boolean ToBoolean() const;
    Number ToNumber() const;
    String ToString() const;
    Object ToObject() const;

  protected:
    napi_env _env;
    napi_value _value;
  };

  class Boolean : public Value {
  public:
    static Boolean New(napi_env env, bool val);

    Boolean();
    Boolean(napi_env env, napi_value value);

    operator bool() const;

    bool Value() const;
  };

  class Number : public Value {
  public:
    static Number New(napi_env env, double val);

    Number();
    Number(napi_env env, napi_value value);

    operator int32_t() const;
    operator uint32_t() const;
    operator int64_t() const;
    operator float() const;
    operator double() const;

    int32_t Int32Value() const;
    uint32_t Uint32Value() const;
    int64_t Int64Value() const;
    float FloatValue() const;
    double DoubleValue() const;
  };

  class String : public Value {
  public:
    static String New(napi_env env, const std::string& value);
    static String New(napi_env env, const std::u16string& value);
    static String New(napi_env env, const char* val, int length = -1);
    static String New(napi_env env, const char16_t* val, int length = -1);

    String();
    String(napi_env env, napi_value value);

    operator std::string() const;
    operator std::u16string() const;
    std::string Utf8Value() const;
    std::u16string Utf16Value() const;
  };

  class Object : public Value {
  public:
    static Object New(napi_env env);

    Object();
    Object(napi_env env, napi_value value);

    Value operator [](const char* name) const;
    Value operator [](const std::string& name) const;
    Value operator [](uint32_t index) const;

    bool Has(napi_value name) const;
    bool Has(Value name) const;
    bool Has(const char* utf8name) const;
    bool Has(const std::string& utf8name) const;
    Value Get(napi_value name) const;
    Value Get(Value name) const;
    Value Get(const char* utf8name) const;
    Value Get(const std::string& utf8name) const;
    void Set(napi_value name, napi_value value);
    void Set(const char* utf8name, napi_value value);
    void Set(const char* utf8name, Value value);
    void Set(const char* utf8name, const char* utf8value);
    void Set(const char* utf8name, bool boolValue);
    void Set(const char* utf8name, double numberValue);
    void Set(const std::string& utf8name, napi_value value);
    void Set(const std::string& utf8name, Value value);
    void Set(const std::string& utf8name, std::string& utf8value);
    void Set(const std::string& utf8name, bool boolValue);
    void Set(const std::string& utf8name, double numberValue);

    bool Has(uint32_t index) const;
    Value Get(uint32_t index) const;
    void Set(uint32_t index, napi_value value);
    void Set(uint32_t index, Value value);
    void Set(uint32_t index, const char* utf8value);
    void Set(uint32_t index, const std::string& utf8value);
    void Set(uint32_t index, bool boolValue);
    void Set(uint32_t index, double numberValue);

    void DefineProperty(const PropertyDescriptor& property);
    void DefineProperties(const std::initializer_list<PropertyDescriptor>& properties);
    void DefineProperties(const std::vector<PropertyDescriptor>& properties);
    bool InstanceOf(const Function& constructor) const;
  };

  template <typename T>
  class External : public Value {
  public:
    static External New(napi_env env,
                        T* data,
                        napi_finalize finalizeCallback = nullptr,
                        void* finalizeHint = nullptr);

    External();
    External(napi_env env, napi_value value);

    T* Data() const;
  };

  class Array : public Object {
  public:
    static Array New(napi_env env);
    static Array New(napi_env env, int length);

    Array();
    Array(napi_env env, napi_value value);

    uint32_t Length() const;
  };

  class ArrayBuffer : public Object {
  public:
    static ArrayBuffer New(napi_env env, size_t byteLength);
    static ArrayBuffer New(napi_env env,
                           void* externalData,
                           size_t byteLength,
                           napi_finalize finalizeCallback = nullptr,
                           void* finalizeHint = nullptr);

    ArrayBuffer();
    ArrayBuffer(napi_env env, napi_value value);

    void* Data();
    size_t ByteLength();

  private:
    void* _data;
    size_t _length;
  };

  class TypedArray : public Object {
  public:
    TypedArray();
    TypedArray(napi_env env, napi_value value);

    napi_typedarray_type TypedArrayType() const;
    uint8_t ElementSize() const;
    size_t ElementLength() const;
    size_t ByteOffset() const;
    size_t ByteLength() const;
    ArrayBuffer ArrayBuffer() const;

    Int8Array AsInt8Array() const;
    Uint8Array AsUint8Array() const;
    Uint8ClampedArray AsUint8ClampedArray() const;
    Int16Array AsInt16Array() const;
    Uint16Array AsUint16Array() const;
    Int32Array AsInt32Array() const;
    Uint32Array AsUint32Array() const;
    Float32Array AsFloat32Array() const;
    Float64Array AsFloat64Array() const;

  protected:
    napi_typedarray_type _type;
    size_t _length;

    TypedArray(napi_env env, napi_value value, napi_typedarray_type type, size_t length);

  private:
    static const napi_typedarray_type unknown_type = static_cast<napi_typedarray_type>(-1);
  };

  template <typename T, napi_typedarray_type A>
  class TypedArray_ : public TypedArray {
  public:
    static TypedArray_ New(napi_env env, size_t elementLength);
    static TypedArray_ New(napi_env env,
                           size_t elementLength,
                           Napi::ArrayBuffer arrayBuffer,
                           size_t bufferOffset);

    TypedArray_();
    TypedArray_(napi_env env, napi_value value);

    T& operator [](size_t index);
    const T& operator [](size_t index) const;

    T* Data();
    const T* Data() const;

  private:
    T* _data;

    TypedArray_(napi_env env, napi_value value, size_t length, T* data);
  };

  class Function : public Object {
  public:
    static Function New(napi_env env,
                        VoidFunctionCallback cb,
                        const char* utf8name = nullptr,
                        void* data = nullptr);
    static Function New(napi_env env,
                        FunctionCallback cb,
                        const char* utf8name = nullptr,
                        void* data = nullptr);
    static Function New(napi_env env,
                        VoidFunctionCallback cb,
                        const std::string& utf8name,
                        void* data = nullptr);
    static Function New(napi_env env,
                        FunctionCallback cb,
                        const std::string& utf8name,
                        void* data = nullptr);

    Function();
    Function(napi_env env, napi_value value);

    Value operator ()(const std::initializer_list<napi_value>& args) const;

    Value Call(const std::initializer_list<napi_value>& args) const;
    Value Call(const std::vector<napi_value>& args) const;
    Value Call(napi_value recv, const std::initializer_list<napi_value>& args) const;
    Value Call(napi_value recv, const std::vector<napi_value>& args) const;

    Value MakeCallback(const std::initializer_list<napi_value>& args) const;
    Value MakeCallback(const std::vector<napi_value>& args) const;
    Value MakeCallback(napi_value recv, const std::initializer_list<napi_value>& args) const;
    Value MakeCallback(napi_value recv, const std::vector<napi_value>& args) const;

    Object New(const std::initializer_list<napi_value>& args) const;
    Object New(const std::vector<napi_value>& args) const;

  private:
    static void VoidFunctionCallbackWrapper(napi_env env, napi_callback_info info);
    static void FunctionCallbackWrapper(napi_env env, napi_callback_info info);

    struct CallbackData {
      CallbackData(VoidFunctionCallback cb, void* data);
      CallbackData(FunctionCallback cb, void* data);
      ~CallbackData() {};
      union {
        VoidFunctionCallback voidFunctionCallback;
        FunctionCallback functionCallback;
      };
      void* data;
    };
  };

  template <typename T>
  class Buffer : public Object {
  public:
    static Buffer<T> New(napi_env env, size_t length);
    static Buffer<T> New(napi_env env, T* data,
                         size_t length,
                         napi_finalize finalizeCallback = nullptr,
                         void* finalizeHint = nullptr);
    static Buffer<T> Copy(napi_env env, const T* data, size_t length);

    Buffer();
    Buffer(napi_env env, napi_value value);
    size_t Length() const;
    T* Data() const;

  private:
    mutable size_t _length;
    mutable T* _data;

    Buffer(napi_env env, napi_value value, size_t length, T* data);
    void EnsureInfo() const;
  };

  /*
   * The NAPI Error class wraps a JavaScript Error object in a way that enables it
   * to traverse a C++ stack and be thrown and caught as a C++ exception.
   *
   * If a NAPI API call fails without executing any JavaScript code (for example due
   * to an invalid argument), then the NAPI wrapper automatically converts and throws
   * the error as a C++ exception of type Napi::Error.
   *
   * If a JavaScript function called by C++ code via NAPI throws a JavaScript exception,
   * then the NAPI wrapper automatically converts and throws it as a C++ exception of type
   * Napi::Error.
   *
   * If a C++ exception of type Napi::Error escapes from a NAPI C++ callback, then
   * the NAPI wrapper automatically converts and throws it as a JavaScript exception.
   *
   * Catching a C++ exception of type Napi::Error also clears the JavaScript exception.
   * Of course it may be then re-thrown, which restores the JavaScript exception.
   *
   * Example 1 - Throwing an exception:
   *
   *   Napi::Env env = ...
   *   throw env.NewError("Example exception");
   *   // Following C++ statements will not be executed.
   *   // The exception will bubble up as a C++ exception of type Napi::Error,
   *   // until it is either caught while still in C++, or else automatically
   *   // re-thrown as a JavaScript exception when the callback returns to JavaScript.
   *
   * Example 2 - Ignoring a NAPI exception:
   *
   *   Napi::Function jsFunctionThatThrows = someObj.AsFunction();
   *   jsFunctionThatThrows({ arg1, arg2 });
   *   // Following C++ statements will not be executed.
   *   // The exception will bubble up as a C++ exception of type Napi::Error,
   *   // until it is either caught while still in C++, or else automatically
   *   // re-thrown as a JavaScript exception when the callback returns to JavaScript.
   *
   * Example 3 - Handling a NAPI exception:
   *
   *   Napi::Function jsFunctionThatThrows = someObj.AsFunction();
   *   try {
   *      jsFunctionThatThrows({ arg1, arg2 });
   *   }
   *   catch (const Napi::Error& e) {
   *     cerr << "Caught JavaScript exception: " + e.what();
   *     // Since the exception was caught here, it will not be re-thrown as
   *     // a JavaScript exception.
   *   }
   */
  class Error : public Object, public std::exception {
  public:
    static Error New(napi_env env);
    static Error New(napi_env env, const char* message);
    static Error New(napi_env env, const std::string& message);

    Error();
    Error(napi_env env, napi_value value);

    std::string Message() const;
    void ThrowAsJavaScriptException() const;

    const char* what() const _NOEXCEPT override;

  protected:
    typedef napi_status (*create_error_fn)(napi_env env, napi_value msg, napi_value* result);

    template <typename TError>
    static TError New(napi_env env,
                      const char* message,
                      size_t length,
                      create_error_fn create_error);

  private:
    std::string _message;
  };

  class TypeError : public Error {
  public:
    static TypeError New(napi_env env, const char* message);
    static TypeError New(napi_env env, const std::string& message);

    TypeError();
    TypeError(napi_env env, napi_value value);
  };

  class RangeError : public Error {
  public:
    static RangeError New(napi_env env, const char* message);
    static RangeError New(napi_env env, const std::string& message);

    RangeError();
    RangeError(napi_env env, napi_value value);
  };

  /*
   * Holds a counted reference to a value; initially a weak reference unless otherwise specified.
   * May be changed to/from a strong reference by adjusting the refcount. The referenced value
   * is not immediately destroyed when the reference count is zero; it merely then eligible for
   * GC if there are no other references to the value.
   */
  template <typename T>
  class Reference {
  public:
    static Reference<T> New(const T& value, int initialRefcount = 0);

    Reference();
    Reference(napi_env env, napi_ref ref);
    ~Reference();

    // A reference can be moved but cannot be copied.
    Reference(Reference<T>&& other);
    Reference<T>& operator =(Reference<T>&& other);
    Reference(const Reference<T>&) = delete;
    Reference<T>& operator =(Reference<T>&) = delete;

    operator napi_ref() const;
    bool operator ==(const Reference<T> &other) const;
    bool operator !=(const Reference<T> &other) const;

    Env Env() const;
    bool IsEmpty() const;

    // Note when getting the value of a Reference it is usually correct to do so
    // within a HandleScope so that the value handle gets cleaned up efficiently.
    T Value() const;

    int Ref();
    int Unref();
    void Reset();
    void Reset(const T& value, int refcount = 0);

    // Call this on a reference that is declared as static data, to prevent its destructor
    // from running at program shutdown time, which would attempt to reset the reference when
    // the environment is no longer valid.
    void SuppressDestruct();

  protected:
    napi_env _env;
    napi_ref _ref;

  private:
    bool _suppressDestruct;
  };

  class ObjectReference: public Reference<Object> {
  public:
    ObjectReference();
    ObjectReference(napi_env env, napi_ref ref);

    // A reference can be moved but cannot be copied.
    ObjectReference(Reference<Object>&& other);
    ObjectReference& operator =(Reference<Object>&& other);
    ObjectReference(ObjectReference&& other);
    ObjectReference& operator =(ObjectReference&& other);
    ObjectReference(const ObjectReference&) = delete;
    ObjectReference& operator =(ObjectReference&) = delete;

    Napi::Value Get(const char* utf8name) const;
    Napi::Value Get(const std::string& utf8name) const;
    void Set(const char* utf8name, napi_value value);
    void Set(const char* utf8name, Napi::Value value);
    void Set(const char* utf8name, const char* utf8value);
    void Set(const char* utf8name, bool boolValue);
    void Set(const char* utf8name, double numberValue);
    void Set(const std::string& utf8name, napi_value value);
    void Set(const std::string& utf8name, Napi::Value value);
    void Set(const std::string& utf8name, std::string& utf8value);
    void Set(const std::string& utf8name, bool boolValue);
    void Set(const std::string& utf8name, double numberValue);

    Napi::Value Get(uint32_t index) const;
    void Set(uint32_t index, const napi_value value);
    void Set(uint32_t index, const Napi::Value value);
    void Set(uint32_t index, const char* utf8value);
    void Set(uint32_t index, const std::string& utf8value);
    void Set(uint32_t index, bool boolValue);
    void Set(uint32_t index, double numberValue);
  };

  class FunctionReference: public Reference<Function> {
  public:
    FunctionReference();
    FunctionReference(napi_env env, napi_ref ref);

    // A reference can be moved but cannot be copied.
    FunctionReference(Reference<Function>&& other);
    FunctionReference& operator =(Reference<Function>&& other);
    FunctionReference(FunctionReference&& other);
    FunctionReference& operator =(FunctionReference&& other);
    FunctionReference(const FunctionReference&) = delete;
    FunctionReference& operator =(FunctionReference&) = delete;

    Napi::Value operator ()(const std::initializer_list<napi_value>& args) const;

    Napi::Value Call(const std::initializer_list<napi_value>& args) const;
    Napi::Value Call(const std::vector<napi_value>& args) const;
    Napi::Value Call(napi_value recv, const std::initializer_list<napi_value>& args) const;
    Napi::Value Call(napi_value recv, const std::vector<napi_value>& args) const;

    Napi::Value MakeCallback(const std::initializer_list<napi_value>& args) const;
    Napi::Value MakeCallback(const std::vector<napi_value>& args) const;
    Napi::Value MakeCallback(napi_value recv, const std::initializer_list<napi_value>& args) const;
    Napi::Value MakeCallback(napi_value recv, const std::vector<napi_value>& args) const;

    Object New(const std::initializer_list<napi_value>& args) const;
    Object New(const std::vector<napi_value>& args) const;
  };

  // Shortcuts to creating a new reference with inferred type and refcount = 0.
  template <typename T> Reference<T> Weak(T value);
  ObjectReference Weak(Object value);
  FunctionReference Weak(Function value);

  // Shortcuts to creating a new reference with inferred type and refcount = 1.
  template <typename T> Reference<T> Persistent(T value);
  ObjectReference Persistent(Object value);
  FunctionReference Persistent(Function value);

  class CallbackInfo {
  public:
    CallbackInfo(napi_env env, napi_callback_info info);
    ~CallbackInfo();

    Env Env() const;
    int Length() const;
    const Value operator [](int index) const;
    Object This() const;
    void* Data() const;
    void SetData(void* data);

  private:
    const int _staticArgCount = 6;
    napi_env _env;
    napi_value _this;
    size_t _argc;
    napi_value* _argv;
    napi_value _staticArgs[6];
    napi_value* _dynamicArgs;
    void* _data;
  };

  class PropertyDescriptor {
  public:
    PropertyDescriptor(napi_property_descriptor desc) : _desc(desc) {}

    operator napi_property_descriptor&() { return _desc; }
    operator const napi_property_descriptor&() const { return _desc; }

  private:
    napi_property_descriptor _desc;
  };

  /*
  * Property descriptor for use with ObjectWrap<T>::DefineClass(). This is different from
  * the standalone PropertyDescriptor because it is specific to each ObjectWrap<T>
  * subclass. This prevents using descriptors from a different class when defining a new class
  * (preventing the callbacks from having incorrect `this` pointers).
  */
  template <typename T>
  class ClassPropertyDescriptor {
  public:
    ClassPropertyDescriptor(napi_property_descriptor desc) : _desc(desc) {}

    operator napi_property_descriptor&() { return _desc; }
    operator const napi_property_descriptor&() const { return _desc; }

  private:
    napi_property_descriptor _desc;
  };

  /*
   * Base class to be extended by C++ classes exposed to JavaScript.
   * Each C++ class instance gets "wrapped" by a JavaScript object.
   *
   * At initialization time, the DefineClass() method must be used to
   * hook up the accessor and method callbacks. It takes a list of
   * property descriptors, which can be constructed via the various
   * static methods on the base class.
   *
   * Example:
   *
   *   class Example: public Napi::ObjectWrap<Example> {
   *   public:
   *     static void Initialize(Napi::Env& env, Napi::Object& target) {
   *       Napi::Function constructor = DefineClass(env, "Example", New, {
   *         InstanceAccessor("value", &GetValue, &SetValue),
   *         InstanceMethod("doSomething", &DoSomething),
   *       });
   *       target.Set("Example", constructor);
   *     }
   *
   *     static Example* New(const Napi::CallbackInfo& info) {
   *       return new Example();
   *     }
   *
   *     Napi::Value GetValue(const Napi::CallbackInfo& info);
   *     void SetValue(const Napi::CallbackInfo& info, const Napi::Value& value);
   *     Napi::Value DoSomething(const Napi::CallbackInfo& info);
   *   }
   */
  template <typename T>
  class ObjectWrap : public Reference<Object> {
  public:
    ObjectWrap();

    static T* Unwrap(Object wrapper);

    // Methods exposed to JavaScript must conform to one of these callback signatures.
    typedef void (*StaticVoidMethodCallback)(const CallbackInfo& info);
    typedef Napi::Value (*StaticMethodCallback)(const CallbackInfo& info);
    typedef Napi::Value (*StaticGetterCallback)(const CallbackInfo& info);
    typedef void (*StaticSetterCallback)(const CallbackInfo& info, const Napi::Value& value);
    typedef void (T::*InstanceVoidMethodCallback)(const CallbackInfo& info);
    typedef Napi::Value (T::*InstanceMethodCallback)(const CallbackInfo& info);
    typedef Napi::Value (T::*InstanceGetterCallback)(const CallbackInfo& info);
    typedef void (T::*InstanceSetterCallback)(const CallbackInfo& info, const Napi::Value& value);

    typedef ClassPropertyDescriptor<T> PropertyDescriptor;

    static Function DefineClass(Napi::Env env,
                                const char* utf8name,
                                const std::initializer_list<PropertyDescriptor>& properties,
                                void* data = nullptr);
    static Function DefineClass(Napi::Env env,
                                const char* utf8name,
                                const std::vector<PropertyDescriptor>& properties,
                                void* data = nullptr);
    static PropertyDescriptor StaticMethod(const char* utf8name,
                                           StaticVoidMethodCallback method,
                                           napi_property_attributes attributes = napi_default,
                                           void* data = nullptr);
    static PropertyDescriptor StaticMethod(const char* utf8name,
                                           StaticMethodCallback method,
                                           napi_property_attributes attributes = napi_default,
                                           void* data = nullptr);
    static PropertyDescriptor StaticAccessor(const char* utf8name,
                                             StaticGetterCallback getter,
                                             StaticSetterCallback setter,
                                             napi_property_attributes attributes = napi_default,
                                             void* data = nullptr);
    static PropertyDescriptor InstanceMethod(const char* utf8name,
                                             InstanceVoidMethodCallback method,
                                             napi_property_attributes attributes = napi_default,
                                             void* data = nullptr);
    static PropertyDescriptor InstanceMethod(const char* utf8name,
                                             InstanceMethodCallback method,
                                             napi_property_attributes attributes = napi_default,
                                             void* data = nullptr);
    static PropertyDescriptor InstanceAccessor(const char* utf8name,
                                               InstanceGetterCallback getter,
                                               InstanceSetterCallback setter,
                                               napi_property_attributes attributes = napi_default,
                                               void* data = nullptr);
    static PropertyDescriptor StaticValue(const char* utf8name,
                                          Napi::Value value,
                                          napi_property_attributes attributes = napi_default);
    static PropertyDescriptor InstanceValue(const char* utf8name,
                                            Napi::Value value,
                                            napi_property_attributes attributes = napi_default);

  private:
    static void ConstructorCallbackWrapper(napi_env env, napi_callback_info info);
    static void StaticVoidMethodCallbackWrapper(napi_env env, napi_callback_info info);
    static void StaticMethodCallbackWrapper(napi_env env, napi_callback_info info);
    static void StaticGetterCallbackWrapper(napi_env env, napi_callback_info info);
    static void StaticSetterCallbackWrapper(napi_env env, napi_callback_info info);
    static void InstanceVoidMethodCallbackWrapper(napi_env env, napi_callback_info info);
    static void InstanceMethodCallbackWrapper(napi_env env, napi_callback_info info);
    static void InstanceGetterCallbackWrapper(napi_env env, napi_callback_info info);
    static void InstanceSetterCallbackWrapper(napi_env env, napi_callback_info info);
    static void FinalizeCallback(napi_env env, void* data, void* hint);

    typedef struct {
      union {
        StaticVoidMethodCallback staticVoidMethodCallback;
        StaticMethodCallback staticMethodCallback;
        struct {
          StaticGetterCallback staticGetterCallback;
          StaticSetterCallback staticSetterCallabck;
        };
        InstanceVoidMethodCallback instanceVoidMethodCallback;
        InstanceMethodCallback instanceMethodCallback;
        struct {
          InstanceGetterCallback instanceGetterCallback;
          InstanceSetterCallback instanceSetterCallback;
        };
      };
      void* data;
    } CallbackData;
  };

  class HandleScope {
  public:
    HandleScope(napi_env env, napi_handle_scope scope);
    explicit HandleScope(Napi::Env env);
    ~HandleScope();

    operator napi_handle_scope() const;

    Env Env() const;

  private:
    napi_env _env;
    napi_handle_scope _scope;
  };

  class EscapableHandleScope {
  public:
    EscapableHandleScope(napi_env env, napi_escapable_handle_scope scope);
    explicit EscapableHandleScope(Napi::Env env);
    ~EscapableHandleScope();

    operator napi_escapable_handle_scope() const;

    Env Env() const;
    Value Escape(napi_value escapee);

  private:
    napi_env _env;
    napi_escapable_handle_scope _scope;
  };

  class AsyncWorker {
  public:
    virtual ~AsyncWorker();

    // An async worker can be moved but cannot be copied.
    AsyncWorker(AsyncWorker&& other);
    AsyncWorker& operator =(AsyncWorker&& other);
    AsyncWorker(const AsyncWorker&) = delete;
    AsyncWorker& operator =(AsyncWorker&) = delete;

    operator napi_async_work() const;

    Env Env() const;

    void Queue();
    void Cancel();

    virtual void Execute() = 0;
    virtual void WorkComplete();

    ObjectReference& Persistent();

  protected:
    explicit AsyncWorker(const Function& callback);

    virtual void OnOK();
    virtual void OnError(Error e);

    void SetError(Error error);

    FunctionReference _callback;
    ObjectReference _persistent;

  private:
    static void OnExecute(napi_env env, void* this_pointer);
    static void OnWorkComplete(napi_env env,
                               napi_status status,
                               void* this_pointer);

    napi_env _env;
    napi_async_work _work;
    Reference<Error> _error;
  };

} // namespace Napi

// Inline implementations of all the above class methods are included here.
#include "napi-inl.h"

#endif // SRC_NAPI_H_
