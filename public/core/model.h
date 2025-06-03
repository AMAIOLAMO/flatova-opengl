#pragma once
#ifndef _FL_CORE_MODEL_H
#define _FL_CORE_MODEL_H

#include <core/primitives.h>

typedef struct Model_t {
    Vertex *verts;
    u32 verts_count;

    u32 *elem_idxs;
    u32 elem_count;
} Model;

// loads a triangulated wavefront obj model from the given path.
// free the model using "model_free"
Model* load_model_obj_tri(const char *path);

// frees the model
void model_free(Model *p_model);

#endif // _FL_CORE_MODEL_H
