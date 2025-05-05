#include <model.h>

#include <fast_obj.h>
#include <string.h>

Model* load_model_obj_tri(const char *path) {
    Model *p_model = malloc(sizeof(Model));

    // quote from fast_obj
    /* Note: a dummy zero-initialized value is added to the first index
       of the positions, texcoords, normals and textures arrays. Hence,
       valid indices into these arrays start from 1, with an index of 0
       indicating that the attribute is not present. */
    fastObjMesh *fo_mesh = fast_obj_read(path);

    // this is an assumption, we will always assume every face given is a triangulated face
    const size_t TRI_VERT_COUNT_PER_FACE = 3;
    const size_t FACE_VERT_SIZE = 3;

    size_t size = fo_mesh->face_count * TRI_VERT_COUNT_PER_FACE;

    p_model->verts = malloc(sizeof(Vertex) * size);
    p_model->verts_count = size;

    memset(p_model->verts, 0, sizeof(Vertex) * size);

    u32 offsets[] = {
        0, 1, 2
    };
    
    assert(arr_size(offsets) == TRI_VERT_COUNT_PER_FACE);

    // f v / vt / vn
    for(size_t f_i = 0; f_i < fo_mesh->face_count; f_i++) {

        for(size_t off_i = 0; off_i < arr_size(offsets); off_i++) {
            u32 i_offset = offsets[off_i];

            fastObjIndex *p_vidx = &fo_mesh->indices[f_i * FACE_VERT_SIZE + i_offset];

            size_t p_idx         = p_vidx->p;
            size_t tex_coord_idx = p_vidx->t;
            size_t norm_idx      = p_vidx->n;

            size_t model_vert_idx = f_i * TRI_VERT_COUNT_PER_FACE + off_i;
            glm_vec3_copy(&fo_mesh->positions[p_idx * 3],         p_model->verts[model_vert_idx].pos);
            glm_vec3_copy(&fo_mesh->normals[norm_idx * 3],        p_model->verts[model_vert_idx].normal);
            glm_vec2_copy(&fo_mesh->texcoords[tex_coord_idx * 2], p_model->verts[model_vert_idx].tex_coord);
        }

    }

    fast_obj_destroy(fo_mesh);

    return p_model;
}

void model_free(Model *p_model) {
    free(p_model->verts);
    free(p_model);
}
