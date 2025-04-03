#pragma once
#ifndef _EDITOR_H
#define _EDITOR_H

#include <utils.h>

#include <fl_ecs.h>

typedef struct FlWidgetCtx_t {
    const char *identifier;
    b8 is_open;
} FlWidgetCtx;

typedef struct hashmap* FlEditorCtx;

FlEditorCtx create_editor_ctx(void);

void editor_ctx_register_widget(FlEditorCtx ctx, const char *identifier);

int editor_ctx_iter(FlEditorCtx ctx, size_t *p_iter, FlWidgetCtx **pp_widget_ctx);

b8 editor_ctx_set_widget_open(FlEditorCtx ctx, const char *identifier, b8 is_open);

b8 editor_ctx_is_widget_open(FlEditorCtx ctx, const char *identifier);

void editor_ctx_free(FlEditorCtx ctx);

typedef struct FlEditorComponents_t {
    FlComponent transform, mesh_render;
} FlEditorComponents;


#endif // _EDITOR_H
