#ifndef SRC_NODE_INTERNALS_H_
#define SRC_NODE_INTERNALS_H_

//
// This is a stripped down shim to allow node_api.cc to build outside of the node source tree.
//

#include "node_version.h"
#include "util-inl.h"
#include <stdio.h>

// Windows 8+ does not like abort() in Release mode
#ifdef _WIN32
#define ABORT_NO_BACKTRACE() raise(SIGABRT)
#else
#define ABORT_NO_BACKTRACE() abort()
#endif

#define ABORT() node::Abort()

#ifdef __GNUC__
#define LIKELY(expr) __builtin_expect(!!(expr), 1)
#define UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#define PRETTY_FUNCTION_NAME __PRETTY_FUNCTION__
#else
#define LIKELY(expr) expr
#define UNLIKELY(expr) expr
#define PRETTY_FUNCTION_NAME ""
#endif

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define CHECK(expr)                                                           \
  do {                                                                        \
    if (UNLIKELY(!(expr))) {                                                  \
      static const char* const args[] = { __FILE__, STRINGIFY(__LINE__),      \
                                          #expr, PRETTY_FUNCTION_NAME };      \
      node::Assert(&args);                                                    \
    }                                                                         \
  } while (0)

#define CHECK_EQ(a, b) CHECK((a) == (b))
#define CHECK_GE(a, b) CHECK((a) >= (b))
#define CHECK_GT(a, b) CHECK((a) > (b))
#define CHECK_LE(a, b) CHECK((a) <= (b))
#define CHECK_LT(a, b) CHECK((a) < (b))
#define CHECK_NE(a, b) CHECK((a) != (b))

#ifdef __GNUC__
#define NO_RETURN __attribute__((noreturn))
#else
#define NO_RETURN
#endif

#ifndef NODE_RELEASE
#define NODE_RELEASE "node"
#endif

namespace node {

// The slightly odd function signature for Assert() is to ease
// instruction cache pressure in calls from ASSERT and CHECK.
NO_RETURN void Abort();
NO_RETURN void Assert(const char* const (*args)[4]);
void DumpBacktrace(FILE* fp);

template <typename T, size_t N>
constexpr size_t arraysize(const T(&)[N]) { return N; }

NO_RETURN void FatalError(const char* location, const char* message);

}  // namespace node

#if NODE_MAJOR_VERSION < 6
namespace v8 {
  namespace Private {
    v8::Local<v8::Name> ForApi(v8::Isolate* isolate, v8::Local<v8::String> key);
  }
}
#define GetPrivate(context, key) Get((context), (key))
#define SetPrivate(context, key, value)                                 \
  DefineOwnProperty((context), (key), (value),                          \
                    static_cast<v8::PropertyAttribute>(v8::DontEnum |   \
                                                       v8::DontDelete | \
                                                       v8::ReadOnly))
#endif // NODE_MAJOR_VERSION < 6

#endif  // SRC_NODE_INTERNALS_H_
