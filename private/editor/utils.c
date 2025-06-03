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
