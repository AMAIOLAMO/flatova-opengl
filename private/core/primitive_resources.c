#include <core/primitive_resources.h>

#include <stdlib.h>
#include <tinydir.h>
#include <tinydir_ext.h>
#include <cxlist.h>

void res_shader_free(void *p_raw) {
    Shader *p_shader = p_raw;
    shader_free(*p_shader);
    free(p_shader);
}

Shader* resources_load_shader_from_files(Resources resources, const char *identifier,
                                         const char *vert_path, const char *frag_path) {
    Shader *p_shader = malloc(sizeof(Shader));

    if(try_load_shader_from_files(p_shader, vert_path, frag_path)) {
        resources_store(
            resources,
            &(Resource){ .id = identifier, .p_raw = p_shader, .type = FL_RES_SHADER_PROG, .free = res_shader_free }
        );

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
    Model *p_model = load_model_obj_tri(obj_path);
    if(p_model)
        resources_store(
            resources,
            &(Resource){ .id = identifier, .p_raw = p_model, .type = FL_RES_MODEL, .free = res_model_free }
        );

    return p_model;
}

void res_texture2d_free(void *p_raw) {
    GLuint *p_tex = p_raw;
    glDeleteTextures(1, p_tex);
    free(p_tex);
}

GLuint* resources_load_tex2d_linear_from_file(Resources resources, const char *identifier,
                                             const char *path, GLenum tex_color) {
    GLuint *p_tex = malloc(sizeof(GLuint));

    if(gl_try_load_texture2d_linear(path, p_tex, tex_color) == false) {
        free(p_tex);

        return NULL;
    }

    resources_store(
        resources,
        &(Resource){ .id = identifier, .p_raw = p_tex, .type = FL_RES_TEXTURE, .free = res_texture2d_free }
    );

    return p_tex;
}

GLuint* resources_load_tex2d_nearest_from_file(Resources resources, const char *identifier,
                                             const char *path, GLenum tex_color) {
    GLuint *p_tex = malloc(sizeof(GLuint));

    if(gl_try_load_texture2d_nearest(path, p_tex, tex_color) == false) {
        free(p_tex);

        return NULL;
    }

    resources_store(
        resources,
        &(Resource){ .id = identifier, .p_raw = p_tex, .type = FL_RES_TEXTURE, .free = res_texture2d_free });
    return p_tex;
}


void resources_idstrings_free(void *p_raw) {
    ResIdStrings *p_strs = p_raw;

    for(size_t i = 0; i < p_strs->len; i++)
        free(p_strs->data[i]);

    list_free(p_strs);
    free(p_strs);
}

ResIdStrings* resources_lazy_get_id_strings(Resources res) {
    const char *ALLOC_RES_STRING_ID = "__ALLOC_RES_STRINGS__";

    // lazy allocation
    ResIdStrings *p_strings = resources_find(res, ALLOC_RES_STRING_ID);

    if(p_strings == NULL) {
        p_strings = malloc(sizeof(ResIdStrings));
        memset(p_strings, 0, sizeof(ResIdStrings));

        resources_store(
            res,
            &(Resource){ .id = ALLOC_RES_STRING_ID, .p_raw = p_strings, .type = FL_RES_OTHER, .free = resources_idstrings_free }
        );

        printf("pstr: %p\n", (void*)p_strings);
    }

    return p_strings;
}

void resources_load_dir_recursive(Resources res, size_t depth, const char *path) {
    tinydir_dir vendor_dir = {0};

    ResIdStrings *p_strings = resources_lazy_get_id_strings(res);

    if(tinydir_open(&vendor_dir, path) == -1) {
        perror("ERROR: cannot open vendor directory!");
    }

    else {
        while(vendor_dir.has_next) {
            tinydir_file file;

            if(tinydir_readfile(&vendor_dir, &file) == -1)
                continue;

            if(file.is_dir) {
                // normal directories (not . and ..)
                if(!tinydir_is_nav(file)) {
                    if(depth > 0)
                        resources_load_dir_recursive(res, depth - 1, file.path);
                }
            }

            const char *PRIMITIVES_PREFIX = "primitives/";
            /*const char *SHADER_PREFIX     = "shaders/";*/
            const char *TEXTURES_PREFIX    = "textures/";

            if(file.is_reg) {
                size_t str_len = strlen(file.name) - strlen(file.extension) + 1;
                if(strcmp(file.extension, "obj") == 0) {
                    str_len += strlen(PRIMITIVES_PREFIX);

                    char *id = malloc(str_len);
                    memset(id, '\0', str_len);

                    strncpy(id, PRIMITIVES_PREFIX, strlen(PRIMITIVES_PREFIX));
                    strncat(id, file.name, strlen(file.name) - strlen(file.extension) - 1);

                    list_append(p_strings, id);

                    resources_load_model_from_obj(res, id, file.path);
                }

                const char *IMG_FORMATS[] = {
                    "png", "jpeg", "jpg"
                };

                for(size_t i = 0; i < arr_size(IMG_FORMATS); i++) {
                    if(strcmp(file.extension, IMG_FORMATS[i]) != 0)
                        continue;
                    // else
                    
                    str_len += strlen(TEXTURES_PREFIX);
                    char *id = malloc(str_len);

                    memset(id, '\0', str_len);
                    strncpy(id, TEXTURES_PREFIX, strlen(TEXTURES_PREFIX));
                    strncat(id, file.name, strlen(file.name) - strlen(file.extension) - 1);

                    list_append(p_strings, id);

                    resources_load_tex2d_nearest_from_file(res, id, file.path, GL_RGBA);

                    break;
                }
            }

            if(tinydir_next(&vendor_dir) == -1)
                perror("ERROR; cannot get next file");
        }
    }

    tinydir_close(&vendor_dir);
}

