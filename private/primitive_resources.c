#include <primitive_resources.h>
#include <stdio.h>

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
