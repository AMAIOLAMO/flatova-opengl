#include <glad/glad.h>

#include <primitives.h>

#include <fast_obj.h>

Vertex prim_cube_vertices[] = {
    { {-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f } },
    { { 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f } },
    { { 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f } },
    { { 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f } },
    { {-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f } },
    { {-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f } },

    { {-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f } },
    { { 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f } },
    { { 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f } },
    { { 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f } },
    { {-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f } },
    { {-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f } },

    { {-0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } },
    { {-0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } },
    { {-0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } },
    { {-0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } },
    { {-0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } },
    { {-0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } },

    { { 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } },
    { { 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } },
    { { 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } },
    { { 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } },
    { { 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } },
    { { 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } },

    { {-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f } },
    { { 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f } },
    { { 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f } },
    { { 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f } },
    { {-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f } },
    { {-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f } },

    { {-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f } },
    { { 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f } },
    { { 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f } },
    { { 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f } },
    { {-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f } },
    { {-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f } }
};

Vertex prim_plane_vertices[] = {
    { {-0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
    { { 0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
    { { 0.5f, 0.0f,  0.5f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
    { { 0.5f, 0.0f,  0.5f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
    { {-0.5f, 0.0f,  0.5f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
    { {-0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } }
};

Vertex* get_prim_cube_vertices(void) {
    return prim_cube_vertices;
}
size_t get_prim_cube_vertices_size(void) {
    return arr_size(prim_cube_vertices);
}

Vertex* get_prim_plane_vertices(void) {
    return prim_plane_vertices;
}
size_t get_prim_plane_vertices_size(void) {
    return arr_size(prim_plane_vertices);
}


void vertex_bind_load_buffers(Vertex *vertices, size_t vert_size, u32 vert_buf, u32 norm_buf, u32 tex_coord_buf) {
    size_t total_byte_size = vert_size * sizeof(Vertex);

    glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buf);
    glBufferData(GL_ARRAY_BUFFER, total_byte_size, vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, norm_buf);
    glBufferData(GL_ARRAY_BUFFER, total_byte_size, vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
    glBufferData(GL_ARRAY_BUFFER, total_byte_size, vertices, GL_DYNAMIC_DRAW);
}
