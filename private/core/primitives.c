#include <glad/glad.h>

#include <core/primitives.h>
#include <core/macros.h>

#include <fast_obj.h>

Vertex prim_plane_vertices[] = {
    { {-0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
    { { 0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
    { { 0.5f, 0.0f,  0.5f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
    { { 0.5f, 0.0f,  0.5f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
    { {-0.5f, 0.0f,  0.5f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
    { {-0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } }
};

Vertex* get_prim_plane_vertices(void) {
    return prim_plane_vertices;
}
size_t get_prim_plane_vertices_size(void) {
    return arr_size(prim_plane_vertices);
}


void vertex_load_buffers(Vertex *vertices, size_t vert_size, u32 vert_buf, u32 norm_buf, u32 tex_coord_buf) {
    size_t total_byte_size = vert_size * sizeof(Vertex);

    glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buf);
    glBufferData(GL_ARRAY_BUFFER, total_byte_size, vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, norm_buf);
    glBufferData(GL_ARRAY_BUFFER, total_byte_size, vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
    glBufferData(GL_ARRAY_BUFFER, total_byte_size, vertices, GL_DYNAMIC_DRAW);
}

VertexPipeline vertex_gen_buffer_arrays(void) {
    VertexPipeline pipeline = {0};
    glGenVertexArrays(1, &pipeline.vert_arr);

    glBindVertexArray(pipeline.vert_arr);

    glGenBuffers(1, &pipeline.vert_buf);

    glBindBuffer(GL_ARRAY_BUFFER, pipeline.vert_buf);
    glVertexAttribPointer(0, sizeof(vec3) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));

    glGenBuffers(1, &pipeline.tex_coord_buf);

    glBindBuffer(GL_ARRAY_BUFFER, pipeline.tex_coord_buf);
    glVertexAttribPointer(1, sizeof(vec2) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));

    glGenBuffers(1, &pipeline.norm_buf);

    glBindBuffer(GL_ARRAY_BUFFER, pipeline.norm_buf);
    glVertexAttribPointer(2, sizeof(vec3) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    return pipeline;
}

