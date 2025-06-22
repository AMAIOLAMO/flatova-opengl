#pragma once
#ifndef _FL_HOTRELOAD_H
#define _FL_HOTRELOAD_H

#include <core/types.h>

typedef int fl_hotreload_result_t;

#define FL_HOTRELOAD_REQUEST 0x1
#define FL_EXIT 0x2

/// A type representing a callback with no arguments
typedef void(*fl_empty_callback_t)(void);

/// sets the global editor hotreload callback
void fl_set_on_request_hot_reload_callback(fl_empty_callback_t callback);

/// requests this session for hotreload
void fl_request_hot_reload(void);

/// returns true if hotreload has been requested this session
b8 fl_is_request_hotreload(void);

#endif // _FL_HOTRELOAD_H
