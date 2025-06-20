#include <core/cross_load.h>

#include <dlfcn.h>
#include <stdlib.h>

fl_dynlib_err_t fl_open_lib(const char *path, fl_dynlib_t *p_handle) {

#if defined(_FL_WIN32)
    *p_handle = LoadLibraryA(path);

#else // POSIX
    *p_handle = dlopen(path, RTLD_NOW);

#endif

    if((*p_handle) == NULL)
        return FL_DYNLIB_ERR_FAIL;

    return FL_DYNLIB_ERR_NONE;
}

void* fl_load_func(fl_dynlib_t handle, const char *sym_name) {

#if defined(_FL_WIN32)
    return GetProcAddress(handle, sym_name);

#else // POSIX
    return dlsym(handle, sym_name);

#endif
}

void fl_close_lib(fl_dynlib_t handle) {

#if defined(_FL_WIN32)
    FreeLibrary(handle);

#else // POSIX
    dlclose(handle);

#endif
}

