#pragma once
#ifndef _FL_CORE_GL_UTILS_H
#define _FL_CORE_GL_UTILS_H

#include <GL/gl.h>
#include <core/types.h>

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

#endif // _FL_CORE_GL_UTILS_H
