#pragma once
#ifndef _UTILS_H
#define _UTILS_H

#include <core/types.h>

#include <cglm/types.h>
#include <GLFW/glfw3.h>

// returns -1 if negative key is pressed, 1 if positive key is pressed, and 0 if both are pressed
float glfw_key_strength(GLFWwindow *p_win, int p_key, int n_key);

// X -> Y -> Z (parent to child relation, where Z is the most inner children)
void euler_radians_transform_xyz(vec3 euler_rads, mat4 applied_mat);

float clampf(float val, float min, float max);

#endif // _UTILS_H
