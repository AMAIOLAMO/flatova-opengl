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

typedef int (*fl_init_func_t )(void *p_state);
typedef int (*fl_run_func_t  )(void *p_state);
typedef int (*fl_close_func_t)(void *p_state);

typedef int (*fl_reload_func_t)(void *p_state);

#define FL_HOTRELOAD_REQUEST 0x1
#define FL_EXIT 0x2

/// sets the global editor hotreload callback
void fl_set_on_request_hot_reload_callback(fl_empty_callback_t callback);

/// requests this session for hotreload
void fl_request_hot_reload(void);

/// returns true if hotreload has been requested this session
b8 fl_is_request_hotreload(void);

#endif // _FL_HOTRELOAD_H
