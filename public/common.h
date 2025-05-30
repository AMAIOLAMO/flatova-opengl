#pragma once
#ifndef _COMMON_H
#define _COMMON_H

#include <cglm/types.h>

#include <model.h>
#include <shader.h>
#include <fl_ecs.h>
#include <camera.h>

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

    FlEntityId selected_entity;

    FlEcsCtx *p_ecs_ctx;
} FlScene;


#endif // _COMMON_H
