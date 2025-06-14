#pragma once
#ifndef _FL_CROSS_LOAD_H
#define _FL_CROSS_LOAD_H

// A cross platform(Windows AND posix supported systems) dynamic library loading

// TODO: Add other ones like FL_POSIX just for completeness
// and give #error when none has matched

#if defined(_FL_WIN32)
    #include <windows.h>
#endif


/// TYPE DEFINITIONS ///
#if defined(_FL_WIN32)
    typedef HMODULE fl_dynlib_t;
#else
    typedef void* fl_dynlib_t;
#endif

typedef unsigned int fl_dynlib_err_t;

enum FlDynamicLibErrors {
    FL_DYNLIB_ERR_NONE,
    FL_DYNLIB_ERR_FAIL
};

/// loads a dynamic library into the given dynamic library pointer
/// returns FL_DYNLIB_ERR_NONE if succeess, and FL_DYNLIB_ERR_FAIL if failed
fl_dynlib_err_t fl_open_lib(const char *path, fl_dynlib_t *p_handle);

/// closes the given dynamic library handle
void fl_close_lib(fl_dynlib_t handle);

/// loads the given symbol name from the given library
/// returns the raw address of the function in memory
void* fl_load_func(fl_dynlib_t handle, const char *sym_name);


#endif // _FL_CROSS_LOAD_H
