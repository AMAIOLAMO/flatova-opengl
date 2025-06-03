#pragma once
#ifndef _EDITOR_H
#define _EDITOR_H

#include <GLFW/glfw3.h>

#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_VARARGS
#include <nuklear.h>

#include <core/camera.h>
#include <core/resources.h>
#include <core/fl_ecs.h>
#include <core/common.h>

#define FL_WIDGET_COMMON      0x0
#define FL_WIDGET_METRICS     0x1
#define FL_WIDGET_SETTINGS    0x2
#define FL_WIDGET_NO_CATEGORY 0x3

typedef struct FlWidgetCtx_t {
    const char *id;
    GLuint *p_icon_tex;
    uint16_t type;
    b8 is_open;
} FlWidgetCtx;

typedef struct FlEditorComponents_t {
    FlComponent transform, mesh_render, dir_light, meta;
} FlEditorComponents;

typedef struct FlEditorWidgetIds_t {
    literal_str scene_hierarchy, cam_properties, res_manager,
    scene_settings, file_browser, entity_inspector,
    console, tutorial;
} FlEditorWidgetIds;

typedef enum FlEditorMode_t {
    FL_EDITOR_VIEW, FL_EDITOR_GRAB
} FlEditorMode;

typedef struct FlEditorCtx_t {
    struct hashmap *widgets;
    FlEditorMode mode;
    GLFWwindow *p_win;
    struct nk_context *p_nk_ctx;
} FlEditorCtx;

// TODO: instead of create editor ctx, we just allow the user create it
// themselves, since we might add more and more into the ctx
// FlEditorCtx create_editor_ctx(void);
struct hashmap* editor_ctx_create_widgets(void);

// TODO: replace all ctx to use pointers instead of just a struct copy
// for API consistency
const char* editor_ctx_register_widget(FlEditorCtx *p_ctx, FlWidgetCtx *p_widget_ctx);

int editor_ctx_iter(FlEditorCtx ctx, size_t *p_iter, FlWidgetCtx **pp_widget_ctx);

b8 editor_ctx_set_widget_open(FlEditorCtx ctx, const char *identifier, b8 is_open);

const FlWidgetCtx* editor_ctx_get_widget(const FlEditorCtx *ctx, const char *identifier);

b8 editor_ctx_is_widget_open(const FlEditorCtx *p_ctx, const char *identifier);

void editor_ctx_free_widgets(struct hashmap *widgets);

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
void render_scene_hierarchy(struct nk_context *p_ctx, FlScene *p_scene, FlEditorComponents *p_comps, Resources resources);

void render_camera_properties(struct nk_context *p_ctx, GLFWwindow *p_win, Camera *p_cam, CameraSettings *p_cam_settings);

void render_editor_metrics(struct nk_context *p_ctx, GLFWwindow *p_win, float dt);

void render_resource_manager(struct nk_context *p_ctx, Resources resources);

void render_scene_settings(struct nk_context *p_ctx, FlScene *p_scene);

void render_tutorial(struct nk_context *p_ctx, vec2 win_size, Resources resources);


void render_main_menubar(struct nk_context *p_ctx, GLFWwindow *p_win, Resources resources, FlEditorCtx *p_editor_ctx);

void render_file_browser(struct nk_context *p_ctx);

void render_entity_inspector(struct nk_context *p_ctx, FlScene *p_scene, Resources resources, FlEditorComponents *p_comps);


#endif // _EDITOR_H
