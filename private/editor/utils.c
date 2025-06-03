#include <cglm/cglm.h>
#include <glad/glad.h>
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
