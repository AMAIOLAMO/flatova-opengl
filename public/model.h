#pragma once
#ifndef _MODEL_H
#define _MODEL_H

#include <primitives.h>

typedef struct Model_t {
    Vertex *verts;
    u32 verts_count;

    u32 *elem_idxs;
    u32 elem_count;
} Model;

// loads a wavefront obj model from the given path.
// free the model using "model_free"
Model* load_model_obj(const char *path);

// frees the model
void model_free(Model *p_model);

#endif // _MODEL_H
