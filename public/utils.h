#pragma once
#ifndef _FL_EDITOR_UTILS_H
#define _FL_EDITOR_UTILS_H

#include <core/types.h>

#include <cglm/types.h>
#include <GLFW/glfw3.h>

// returns -1 if negative key is pressed, 1 if positive key is pressed, and 0 if both are pressed
float glfw_key_strength(GLFWwindow *p_win, int p_key, int n_key);

#endif // _FL_EDITOR_UTILS_H
