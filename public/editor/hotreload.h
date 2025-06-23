#pragma once
#ifndef _FL_HOTRELOAD_H
#define _FL_HOTRELOAD_H

#include <core/types.h>
#include <stddef.h>

typedef int fl_hotreload_result_t;

/// A type representing a callback with no arguments
typedef void(*fl_empty_callback_t)(void);

typedef size_t (*fl_state_byte_size_func_t)(void);

typedef void (*fl_set_on_request_hot_reload_callback_func_t)(fl_empty_callback_t callback);

/// called first time when the editor has started, this will not be called in reload
typedef int (*fl_init_func_t )(void *p_state);

/// called everytime hotreloading has occurred
typedef int (*fl_reload_func_t)(void *p_state);

/// main loop of running, returns the state on whether the editor runner should hotreload the editor
/// i.e. return FL_EXIT if the editor wishes to exit, check FL RUN EXIT CODES defined below for more information
typedef int (*fl_run_func_t  )(void *p_state);

/// ran by the editor runner when the editor closes normally, signaled by FL_EXIT,
/// it will not be called when the editor reloads
typedef int (*fl_close_func_t)(void *p_state);

/// similar to fl_close, but instead calls only before hotreloading happens, preferably for cleaning up resources
typedef int (*fl_hotreload_close_func_t)(void *p_state);


/// FL RUN EXIT CODES
#define FL_HOTRELOAD_REQUEST 0x1
#define FL_EXIT 0x2

/// sets the global editor hotreload callback
void fl_set_on_request_hot_reload_callback(fl_empty_callback_t callback);

/// requests this session for hotreload
void fl_request_hot_reload(void);

/// returns true if hotreload has been requested this session
b8 fl_is_request_hotreload(void);

#endif // _FL_HOTRELOAD_H
