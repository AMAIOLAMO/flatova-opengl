#pragma once
#ifndef _FL_CORE_PRIMITIVE_RESOURCES_H
#define _FL_CORE_PRIMITIVE_RESOURCES_H

#include <core/shader.h>
#include <core/resources.h>
#include <core/model.h>

// ======== COMMON RESOURCES LOADING ======== //

// loads a shader resource from a vertex and fragment path into memory.
// automatically deallocates when resources_free has been called.
Shader* resources_load_shader_from_files(
    Resources resources, const char *identifier,
    const char *vert_path, const char *frag_path
);

// loads a model resource from obj file path into memory.
// automatically deallocates when resources_free has been called.
Model* resources_load_model_from_obj(
    Resources resources, const char *identifier,
    const char *obj_path
);

// loads a texture2d resource from an image file path into memory and apply linear filter.
// automatically deallocates when resources_free has been called.
GLuint* resources_load_tex2d_linear_from_file(
    Resources resources, const char *identifier,
    const char *path, GLenum tex_color
);

// loads a texture2d resource from an image file path into memory and apply nearest filter.
// automatically deallocates when resources_free has been called.
GLuint* resources_load_tex2d_nearest_from_file(
    Resources resources, const char *identifier,
    const char *path, GLenum tex_color);

typedef struct ResIdStrings_t {
    size_t len, cap;
    char **data;
} ResIdStrings;

void resources_load_dir_recursive(Resources res, size_t depth, const char *path);

ResIdStrings* resources_lazy_get_id_strings(Resources res);

#endif // _FL_CORE_PRIMITIVE_RESOURCES_H

