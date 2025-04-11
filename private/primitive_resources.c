#include <primitive_resources.h>

#include <stdlib.h>

void res_shader_free(void *p_raw) {
    Shader *p_shader = p_raw;
    shader_free(*p_shader);
    free(p_shader);
}

Shader* resources_load_shader_from_files(Resources resources, const char *identifier,
                                         const char *vert_path, const char *frag_path) {
    Shader *p_shader = malloc(sizeof(Shader));

    if(try_load_shader_from_files(p_shader, vert_path, frag_path)) {
        resources_store_auto(resources, identifier, p_shader, res_shader_free);

        return p_shader;
    }

    return NULL;
}

void res_model_free(void *p_raw) {
    Model *p_model = p_raw;
    model_free(p_model);
}

Model* resources_load_model_from_obj(
    Resources resources, const char *identifier,
    const char *obj_path
) {
    Model *p_model = load_model_obj(obj_path);
    if(p_model)
        resources_store_auto(resources, identifier, p_model, res_model_free);

    return p_model;
}



