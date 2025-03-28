#include <editor.h>

#include <hashmap.h>
#include <string.h>

static int fl_widget_ctx_cmp(const void *a, const void *b, void *udata) {
    (void) udata;
    const FlWidgetCtx *ra = a;
    const FlWidgetCtx *rb = b;

    return strcmp(ra->identifier, rb->identifier);
}

static uint64_t fl_widget_ctx_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const FlWidgetCtx *res = item;
    // default hash
    return hashmap_sip(res->identifier, strlen(res->identifier), seed0, seed1);
}

FlEditorCtx create_editor_ctx(void) {
    return hashmap_new(sizeof(FlWidgetCtx), 0, 0, 0, fl_widget_ctx_hash, fl_widget_ctx_cmp, NULL, NULL);
}

void editor_ctx_register_widget(FlEditorCtx ctx, const char *identifier) {
    hashmap_set(ctx, &(FlWidgetCtx){.identifier = identifier, .is_open = false});
}

int editor_ctx_iter(FlEditorCtx ctx, size_t *p_iter, FlWidgetCtx **pp_widget_ctx) {
    return hashmap_iter(ctx, p_iter, (void**)pp_widget_ctx);
}

b8 editor_ctx_set_widget_open(FlEditorCtx ctx, const char *identifier, b8 is_open) {
    const FlWidgetCtx *scene_widget = hashmap_get(ctx, &(FlWidgetCtx){ .identifier = identifier });
    if(!scene_widget)
        return false;

    hashmap_set(ctx, &(FlWidgetCtx){.identifier = identifier, .is_open = is_open});
    return true;
}

b8 editor_ctx_is_widget_open(FlEditorCtx ctx, const char *identifier) {
    const FlWidgetCtx *scene_widget = hashmap_get(ctx, &(FlWidgetCtx){ .identifier = identifier });
    
    return scene_widget && scene_widget->is_open;
}

void editor_ctx_free(FlEditorCtx ctx) {
    hashmap_free(ctx);
}
