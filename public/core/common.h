#pragma once
#ifndef _COMMON_H
#define _COMMON_H

#include <cglm/types.h>

#include <core/model.h>
#include <core/shader.h>
#include <core/fl_ecs.h>
#include <core/camera.h>

typedef struct FlTransform_t {
    vec3 pos, rot, scale;
} FlTransform;

typedef struct FlMeshRender_t {
    const Model *p_model;
    const Shader *p_shader;

    GLuint *p_diffuse_tex;
    GLuint *p_specular_tex;
    float specular_factor;
} FlMeshRender;

#define FL_META_NAME_LEN_MAX 255

typedef struct FlMeta_t {
    char name[FL_META_NAME_LEN_MAX + 1];
} FlMeta;

typedef struct FlGlobalLight_t {
    vec3 dir;
    vec3 color;
} FlGlobalLight;

void transform_apply(FlTransform transform, mat4 applied_mat);


typedef struct FlScene_t {
    // TODO: maybe put some of these in Environmental settings?
    /*EnvironmentSettings env_settings;*/
    vec4 clear_color;
    vec3 ambient_color;

    b8 wireframe_mode;

    // TODO: selected entity is not supposed to be in scene(its related to the editor)
    FlEntityId selected_entity;

    FlEcsCtx *p_ecs_ctx;
} FlScene;

// X -> Y -> Z (parent to child relation, where Z is the most inner children)
void euler_radians_transform_xyz(vec3 euler_rads, mat4 applied_mat);

float clampf(float val, float min, float max);

#endif // _COMMON_H
