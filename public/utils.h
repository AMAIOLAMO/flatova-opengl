#pragma once
#ifndef _UTILS_H
#define _UTILS_H

#include <core/types.h>

#include <cglm/types.h>
#include <GLFW/glfw3.h>

#define arr_size(ARR) (sizeof(ARR) / sizeof(ARR[0]))

#define true 1
#define false 0

#define FL_PI 3.14159265358979f
#define FL_TAU (FL_PI * 2)

#define DEG2RAD (FL_PI / 180.0f)

// returns -1 if negative key is pressed, 1 if positive key is pressed, and 0 if both are pressed
float glfw_key_strength(GLFWwindow *p_win, int p_key, int n_key);

// allocates a buffer to the buffer referenced by "out_content", and sets the "out_size" to be content size.
// returns true if file loaded succesfully, false otherwise.
b8 try_load_file_text(const char *file_path, char **out_content, size_t *out_size);

// frees the buffer "out_content" returned by try_load_file_text
void file_text_free(char *content);

typedef struct LoadImgAsTexture2dInfo_t {
    unsigned char *img_data;
    int width, height;
    GLenum tex_colors, src_colors, data_type;
} LoadImgAsTexture2dInfo;

// Simplifies loading of a texture2d from raw bytes, does not apply any wrapping and filtering settings.
// This also generates mipmaps levels for the image.
void gl_load_img_as_texture2d(LoadImgAsTexture2dInfo *p_info, GLuint *p_tex);

// Simplifies loading of a texture2d from a file, returns true if succeeded to load, does not apply any wrapping and filtering settings.
// This also generates mipmaps levels for the image.
void gl_load_img_as_texture2d(LoadImgAsTexture2dInfo *p_info, GLuint *p_tex);

// Simplifies loading of a texture2d from a file, returns true if succeeded to load, uses linear mipmap levels
b8 gl_try_load_texture2d_linear(const char *file_path, GLuint *p_tex, GLenum tex_colors);

// Simplifies loading of a texture2d from a file, returns true if succeeded to load, uses nearest mipmap levels
b8 gl_try_load_texture2d_nearest(const char *file_path, GLuint *p_tex, GLenum tex_colors);

// X -> Y -> Z (parent to child relation, where Z is the most inner children)
void euler_radians_transform_xyz(vec3 euler_rads, mat4 applied_mat);

float clampf(float val, float min, float max);

#endif // _UTILS_H
