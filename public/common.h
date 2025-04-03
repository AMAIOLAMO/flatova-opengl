#pragma once
#ifndef _COMMON_H
#define _COMMON_H

#include <cglm/types.h>

#include <model.h>
#include <shader.h>
#include <fl_ecs.h>

typedef struct Transform_t {
    vec3 pos, rot, scale;
} Transform;

typedef struct MeshRender_t {
    const Model *p_model;
    const Shader *p_shader;
    vec3 albedo_color;
} MeshRender;

void transform_apply(Transform transform, mat4 applied_mat);


typedef struct Scene_t {
    vec4 clear_color;
    b8 wireframe_mode;
    vec3 light_pos;

    FlEcsCtx *p_ecs_ctx;
} Scene;

Scene create_scene(const vec4 clear_color, FlEcsCtx *p_ctx);

void scene_free(Scene *p_scene);


typedef struct FlScaling_t {
    vec2 dpi_scale;
} FlScaling;

void scaling_set_dpi(FlScaling *p_handle, float xscale, float yscale);


#endif // _COMMON_H
