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

void gl_load_img_as_texture2d(LoadImgAsTexture2dInfo *p_info, GLuint *p_tex) {
    assert(p_info->img_data);

    glGenTextures(1, p_tex);
    glBindTexture(GL_TEXTURE_2D, *p_tex);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, p_info->tex_colors, p_info->width, p_info->height,
        0, p_info->src_colors, p_info->data_type, p_info->img_data
    );
    glGenerateMipmap(GL_TEXTURE_2D);
}

b8 gl_try_load_texture2d(const char *file_path, GLuint *p_tex, GLenum tex_colors) {
    int width, height, nrchannels;

    unsigned char *data = stbi_load(file_path, &width, &height, &nrchannels, 0);

    GLenum src_colors = GL_RGBA;
    assert(nrchannels >= 2 && nrchannels <= 4 && "ERROR: loaded texture must have number of channels within 2 ~ 4 inclusive");

    switch(nrchannels) {
        case 2:
            src_colors = GL_RG;
            break;
        case 3:
            src_colors = GL_RGB;
            break;
    }

    if(data) {
        LoadImgAsTexture2dInfo info = {0};
        info.img_data   = data;
        info.tex_colors = tex_colors;
        info.src_colors = src_colors;
        info.data_type  = GL_UNSIGNED_BYTE;
        info.width      = width;
        info.height     = height;

        gl_load_img_as_texture2d(&info, p_tex);

        stbi_image_free(data);
        return true;
    }
    // else
    
    printf("Error: cannot load image\n");
    return false;
}

b8 gl_try_load_texture2d_linear(const char *file_path, GLuint *p_tex, GLenum tex_colors) {
    if(gl_try_load_texture2d(file_path, p_tex, tex_colors) == false)
        return false;
    // else
    
    glBindTexture(GL_TEXTURE_2D, *p_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return true;
}

b8 gl_try_load_texture2d_nearest(const char *file_path, GLuint *p_tex, GLenum tex_colors) {
    if(gl_try_load_texture2d(file_path, p_tex, tex_colors) == false)
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

inline float clampf(float val, float min, float max) {
    if(val < min) return min;
    if(val > max) return max;

    return val;
}
