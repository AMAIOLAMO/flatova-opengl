#include <editor/hotreload.h>
#include <stdlib.h>

fl_empty_callback_t g_hotreload_callback = NULL;
b8 g_request_hotreload = false;

b8 fl_is_request_hotreload(void) {
    return g_request_hotreload;
}

void fl_set_on_request_hot_reload_callback(fl_empty_callback_t callback) {
    g_hotreload_callback = callback;
}
void fl_request_hot_reload(void) {
    if(g_hotreload_callback != NULL)
        g_hotreload_callback();
    // else

    g_request_hotreload = true;
}
