#include <stdio.h>
#include <stdlib.h>
#include <node.h>
#include <dlfcn.h>

NODE_C_CTOR(load_napi_module) {
  char *module_name = getenv("NAPI_MODULE_FILENAME");
  if (module_name == NULL) {
    fprintf(stderr,
      "load_napi_module: not loading module with NULL module name\n");
  } else if (dlopen(module_name, RTLD_LAZY | RTLD_DEEPBIND) == NULL) {
    fprintf(stderr, "load_napi_module: Failed to load %s: %s\n", module_name,
      dlerror());
  }
}
