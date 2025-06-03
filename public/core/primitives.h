#pragma once
#ifndef _PRIMITIVES_H
#define _PRIMITIVES_H

#include <cglm/cglm.h>

#include <core/types.h>
#include <GL/gl.h>

typedef struct Vertex_t {
    vec3 pos;
    vec3 normal;
    vec2 tex_coord;
} Vertex;

Vertex* get_prim_plane_vertices(void);
size_t  get_prim_plane_vertices_size(void);

void vertex_load_buffers(Vertex *vertices, size_t total_byte_size, u32 vert_buf, u32 norm_buf, u32 tex_coord_buf);

typedef struct VertexPipeline_t {
    GLuint vert_arr, vert_buf, tex_coord_buf, norm_buf;
} VertexPipeline;

VertexPipeline vertex_gen_buffer_arrays(void);

#endif // _PRIMITIVES_H
