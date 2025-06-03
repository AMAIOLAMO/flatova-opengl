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


