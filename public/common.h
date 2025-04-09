#pragma once
#ifndef _COMMON_H
#define _COMMON_H

#include <cglm/types.h>

#include <model.h>
#include <shader.h>
#include <fl_ecs.h>
#include <camera.h>

typedef struct Transform_t {
    vec3 pos, rot, scale;
} Transform;

typedef struct MeshRender_t {
    const Model *p_model;
    const Shader *p_shader;
    vec3 albedo_color;
    vec3 specular_color;
    float specular_factor;
} MeshRender;

void transform_apply(Transform transform, mat4 applied_mat);


typedef struct Scene_t {
    /*EnvironmentSettings env_settings;*/
    vec3 light_pos;

    vec4 clear_color;
    vec3 ambient_color;

    b8 wireframe_mode;

    FlEcsCtx *p_ecs_ctx;
} Scene;

void scene_free(Scene *p_scene);


#endif // _COMMON_H
