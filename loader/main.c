#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#define NAPI_MODULE_FILE_NAME "NAPI_MODULE_FILE_NAME"
#define NAPI_IMPLEM_FILE_NAME "NAPI_IMPLEM_FILE_NAME"

int load_one(char* filename, int flags) {
  void* handle = dlopen(filename, flags);
  if (handle == NULL) {
    fprintf(stderr, "Loading %s: %s\n", filename, dlerror());
    return -1;
  }
  return 0;
}

void load_napi_module(void) __attribute__((constructor));
void load_napi_module(void) {
  char* module_file_name = getenv(NAPI_MODULE_FILE_NAME);
  char* implem_file_name = getenv(NAPI_IMPLEM_FILE_NAME);

  if (module_file_name != NULL && implem_file_name != NULL) {
    if (load_one(implem_file_name, RTLD_LAZY | RTLD_GLOBAL) == 0) {
      load_one(module_file_name, RTLD_LAZY);
    }
  }
}
