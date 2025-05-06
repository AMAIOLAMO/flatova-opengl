#pragma once
#ifndef _EDITOR_H
#define _EDITOR_H

#include <utils.h>

#include <camera.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_VARARGS
#include <nuklear.h>
#include <resources.h>

#include <fl_ecs.h>
#include <common.h>

typedef struct FlWidgetCtx_t {
    const char *identifier;
    GLuint *p_icon_tex;
    b8 is_open;
} FlWidgetCtx;

typedef enum FlEditorMode_t {
    FL_EDITOR_VIEW, FL_EDITOR_GRAB
} FlEditorMode;

typedef struct FlEditorCtx_t {
    struct hashmap *widgets;
    FlEditorMode mode;
} FlEditorCtx;

FlEditorCtx create_editor_ctx(void);

void editor_ctx_register_widget(FlEditorCtx ctx, const char *identifier, GLuint *p_icon_tex);

int editor_ctx_iter(FlEditorCtx ctx, size_t *p_iter, FlWidgetCtx **pp_widget_ctx);

b8 editor_ctx_set_widget_open(FlEditorCtx ctx, const char *identifier, b8 is_open);

b8 editor_ctx_is_widget_open(FlEditorCtx ctx, const char *identifier);

void editor_ctx_free(FlEditorCtx ctx);

typedef struct FlEditorComponents_t {
    FlComponent transform, mesh_render, dir_light, meta;
} FlEditorComponents;

typedef struct FlScaling_t {
    vec2 dpi_scale;
} FlScaling;

void scaling_set_dpi(FlScaling *p_handle, float xscale, float yscale);

void g_scaling_set_dpi(float xscale, float yscale);

float g_scaling_x(float xscale);
float g_scaling_y(float yscale);

void fl_xyz_widget(struct nk_context *p_ctx, const char *name, vec3 p_pos, float min, float max);

void fl_combo_color_picker(struct nk_context *p_ctx, struct nk_colorf *p_color);

// hierarchy
void render_scene_hierarchy(struct nk_context *p_ctx, Scene *p_scene, FlEditorComponents *p_comps,
                            Resources resources, FlEntityId *p_chosen_entity);

void render_camera_properties(struct nk_context *p_ctx, GLFWwindow *p_win, Camera *p_cam, CameraSettings *p_cam_settings);

void render_editor_metrics(struct nk_context *p_ctx, GLFWwindow *p_win, float dt);

void render_resource_viewer(struct nk_context *p_ctx, Resources resources);

void render_scene_settings(struct nk_context *p_ctx, Scene *p_scene);


void render_main_menubar(struct nk_context *p_ctx, GLFWwindow *p_win, Resources resources, FlEditorCtx *p_editor_ctx);

void render_file_browser(struct nk_context *p_ctx);

void render_entity_inspector(struct nk_context *p_ctx, Scene *p_scene, Resources resources,
                             FlEditorComponents *p_comps, FlEntityId *p_chosen_entity);


#endif // _EDITOR_H
