#pragma once
#ifndef _SHADER_H
#define _SHADER_H

#include <utils.h>

typedef struct Shader_t {
    GLuint vert, frag;
    GLuint program;
} Shader;

b8 try_compile_shader_with_err(int type, const char **p_src, GLuint *out_shader, char *compile_log);

b8 shader_load(const char **p_vert_src, const char **p_frag_src, Shader *out_shader);

void shader_free(const Shader shader);

void shader_use(const Shader shader);


GLuint shader_get_uniform_loc(const Shader shader, const char *name);

void shader_set_uniform_1i(const Shader shader, const char *name, int value);

void shader_set_uniform_1f(const Shader shader, const char *name, float value);

void shader_set_uniform_3f(const Shader shader, const char *name, vec3 value);

void shader_set_uniform_mat4fv(const Shader shader, const char *name, mat4 value);

b8 try_load_shader_from_files(Shader *p_shader, const char *vert_path, const char *frag_path);

#endif // _SHADER_H
