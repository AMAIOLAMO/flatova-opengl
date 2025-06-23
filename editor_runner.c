#include <stdio.h>
#include <stdlib.h>

#include <core/cross_load.h>
#include <core/utils.h>
#include <editor/hotreload.h>

void on_request_hot_reload(void) {
    printf("[Editor runner] hotreload requested\n");
}

#define REPL_FUNC_LIST \
    REPL(fl_init) \
    REPL(fl_run) \
    REPL(fl_close) \
    REPL(fl_reload) \
    REPL(fl_state_byte_size) \
    REPL(fl_hotreload_close) \
    REPL(fl_set_on_request_hot_reload_callback)

#define REPL(FUNC_NAME) \
FUNC_NAME##_func_t FUNC_NAME##_func = NULL;
REPL_FUNC_LIST
#undef REPL

int load_editor_funcs(fl_dynlib_t *p_handle) {
    // TODO: this file path is a hack, it's best to dynamically accept it instead
    if(fl_open_lib("./build/libflatova_editor.so", p_handle) == FL_DYNLIB_ERR_FAIL) {
        printf("Failed to load editor at libflatova_editor.so!\n");
        return -1;
    }
    // else

#define REPL(FUNC_NAME) \
FUNC_NAME##_func = (FUNC_NAME##_func_t)fl_load_func(*p_handle, #FUNC_NAME); \
    if(FUNC_NAME##_func == NULL) { \
        printf("Failed to load " #FUNC_NAME " func!\n"); \
        fl_close_lib(*p_handle); \
        return -1; \
    }
    REPL_FUNC_LIST
    #undef REPL

    return 0;
}


int main(void) {
    fl_dynlib_t lib;

    void *p_hotreload_state = NULL;
    size_t current_state_size = 0;

    b8 hotreload = false;

    while(true) {
        if(load_editor_funcs(&lib) == -1) {
            printf("[Editor Runner] Failed to load library!\n");
            return -1;
        }

        if(current_state_size != fl_state_byte_size_func()) {
            printf("[Editor Runner] State size changed from %zu to %zu\n", current_state_size, fl_state_byte_size_func());
            current_state_size = fl_state_byte_size_func();
            p_hotreload_state = realloc(p_hotreload_state, fl_state_byte_size_func());
        }

        if(hotreload == true) {
            printf("[Editor Runner] fl_reload_func called\n");
            fl_reload_func(p_hotreload_state);
        }
        else {
            printf("[Editor Runner] fl_init_func called\n");
            fl_init_func(p_hotreload_state);
        }

        fl_set_on_request_hot_reload_callback_func(on_request_hot_reload);

        printf("[Editor Runner] fl_run_func called\n");
        int exit_code = fl_run_func(p_hotreload_state);

        if(exit_code == FL_EXIT) {
            printf("[Editor Runner] exiting...\n");
            printf("[Editor Runner] fl_close_func called\n");
            fl_close_func(p_hotreload_state);
            break;
        }
        else if(exit_code == FL_HOTRELOAD_REQUEST) {
            printf("[Editor Runner] hotreload request detected, reloading...\n");
            printf("[Editor Runner] fl_hotreload_close_func called\n");
            fl_hotreload_close_func(p_hotreload_state);
            hotreload = true;
        }
        else {
            printf("[Editor Runner] unsupported exit code: %d, aborting...\n", exit_code);
            printf("[Editor Runner] fl_close_func called\n");
            fl_close_func(p_hotreload_state);
            break;
        }

        fl_close_lib(lib);
    }

    free(p_hotreload_state);

    printf("[Editor Runner] exited.\n");
    return 0;

}
