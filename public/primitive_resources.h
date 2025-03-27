#pragma once
#ifndef _PRIMITIVE_RESOURCES_H
#define _PRIMITIVE_RESOURCES_H

#include <shader.h>
#include <resources.h>

// ======== COMMON RESOURCES LOADING ======== //

// loads a shader resource from a vertex and fragment path into memory.
// automatically deallocates when resources_free has been called.
Shader* resources_load_shader_from_files(
    Resources resources, const char *identifier,
    const char *vert_path, const char *frag_path
);

#endif // _PRIMITIVE_RESOURCES_H

