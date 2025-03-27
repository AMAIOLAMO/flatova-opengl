#include <cglm/cglm.h>
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>
#include <stb_image.h>

float glfw_key_strength(GLFWwindow *p_win, int p_key, int n_key) {
    float strength = 0;

    if(glfwGetKey(p_win, p_key) == GLFW_PRESS)
        strength += 1;

    if(glfwGetKey(p_win, n_key) == GLFW_PRESS)
        strength -= 1;

    return strength;
}

b8 try_load_file_text(const char *file_path, char **out_content, size_t *out_size) {
    FILE *file = fopen(file_path, "r");

    *out_content = NULL;
    *out_size = 0;

    if(file == NULL)
        return false;

    #define READ_CHUNK_SIZE 4
    
    char buf[READ_CHUNK_SIZE + 1] = {0};

    while(true) {
        size_t read_size = fread(&buf, sizeof(buf[0]), READ_CHUNK_SIZE, file);

        if(ferror(file)) {
            free(*out_content);
            *out_content = NULL;
            return false;
        }
        // else

        size_t new_size = read_size + *out_size + 1;

        *out_content = realloc(*out_content, new_size);

        memcpy(*out_content + *out_size, buf, read_size);
        (*out_content)[new_size - 1] = '\0';

        (*out_size) += read_size;

        if(feof(file))
            break;
    }

    fclose(file);
    return true;
}

void file_text_free(char *content) {
    free(content);
}

b8 gl_try_load_texture2d(const char *file_path, GLuint *p_tex, GLenum tex_colors, GLenum src_colors, GLenum data_type) {
    glGenTextures(1, p_tex);
    glBindTexture(GL_TEXTURE_2D, *p_tex);

    int width, height, nrchannels;

    unsigned char *data = stbi_load(file_path, &width, &height, &nrchannels, 0);

    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, tex_colors, width, height, 0, src_colors, data_type, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        return true;
    }
    // else
    
    printf("Error: cannot load image\n");
    return false;
}

b8 gl_try_load_texture2d_linear(const char *file_path, GLuint *p_tex, GLenum tex_colors, GLenum src_colors, GLenum data_type) {
    if(gl_try_load_texture2d(file_path, p_tex, tex_colors, src_colors, data_type) == false)
        return false;
    // else
    
    glBindTexture(GL_TEXTURE_2D, *p_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return true;
}

b8 gl_try_load_texture2d_nearest(const char *file_path, GLuint *p_tex, GLenum tex_colors, GLenum src_colors, GLenum data_type) {
    if(gl_try_load_texture2d(file_path, p_tex, tex_colors, src_colors, data_type) == false)
        return false;
    // else
    
    glBindTexture(GL_TEXTURE_2D, *p_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return true;
}

void euler_radians_transform_xyz(vec3 euler_rads, mat4 applied_mat) {
    glm_rotate_z(applied_mat, euler_rads[2], applied_mat);
    glm_rotate_y(applied_mat, euler_rads[1], applied_mat);
    glm_rotate_x(applied_mat, euler_rads[0], applied_mat);
}
