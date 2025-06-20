#include <stdio.h>

#include <core/cross_load.h>
#include <core/utils.h>
#include <stdlib.h>

// TODO: put all of these definitions in another separate header file instead
typedef size_t (*fl_state_byte_size_func_t)(void);
typedef void(*fl_empty_callback_t)(void);

typedef void (*fl_set_on_request_hot_reload_callback_func_t)(fl_empty_callback_t callback);

typedef int (*fl_init_func_t)(void *p_state);
typedef int (*fl_run_func_t)(void *p_state);
typedef int (*fl_close_func_t)(void *p_state);

typedef int (*fl_reload_func_t)(void *p_state);

void on_request_hot_reload(void) {
    printf("[Editor runner] hotreload requested, not implemented yet\n");
}

int main(void) {
    fl_dynlib_t lib;

    // TODO: this file path is a hack, it's best to dynamically accept it instead
    if(fl_open_lib("./build/libflatova_editor.so", &lib) == FL_DYNLIB_ERR_FAIL) {
        printf("Failed to load editor at libflatova_editor.so!\n");
        return -1;
    }
    // else

    // TODO: use X macro list replace to shorten this code (also include if checks)
    fl_run_func_t run_func = (fl_run_func_t)fl_load_func(
        lib, "fl_run"
    );
    fl_state_byte_size_func_t state_byte_size_func = (fl_state_byte_size_func_t)fl_load_func(
        lib, "fl_state_byte_size"
    );
    fl_init_func_t init_func = (fl_init_func_t)fl_load_func(
        lib, "fl_init"
    );
    fl_close_func_t close_func = (fl_close_func_t)fl_load_func(
        lib, "fl_close"
    );

    fl_reload_func_t reload_func = (fl_reload_func_t)fl_load_func(
        lib, "fl_reload"
    );

    (void) reload_func;

    fl_set_on_request_hot_reload_callback_func_t set_on_request_hot_reload_callback_func = (fl_set_on_request_hot_reload_callback_func_t)fl_load_func(
        lib, "fl_set_on_request_hot_reload_callback"
    );

    if(run_func == NULL) {
        printf("Failed to load fl_run func!\n");
        fl_close_lib(lib);
        return -1;
    }
    // else
    
    void *p_hotreload_state = NULL;

    p_hotreload_state = realloc(p_hotreload_state, state_byte_size_func());

    init_func(p_hotreload_state);

    set_on_request_hot_reload_callback_func(on_request_hot_reload);

    run_func(p_hotreload_state);
    close_func(p_hotreload_state);

    free(p_hotreload_state);

    fl_close_lib(lib);
    return 0;
}
