#ifndef DDL_HIAPPEVENT
#define DDL_HIAPPEVENT

#include <dlfcn.h>
using LIBHANDLE =void*;
#define LOAD_LIB(libPath) dlopen(libPath, RTLD_LAZY | RTLD_LOCAL)
#define CLOSE_LIB(libHandle) dlclose(libHandle)
#define LOAD_SYM(libHandle, symbol) dlsym(libHandle, symbol)
#define LOAD_ERROR() dlerror()

#endif // DDL_HIAPPEVENT