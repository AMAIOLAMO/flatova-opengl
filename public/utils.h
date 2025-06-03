#pragma once
#ifndef _UTILS_H
#define _UTILS_H

#include <core/types.h>

#include <cglm/types.h>
#include <GLFW/glfw3.h>

// returns -1 if negative key is pressed, 1 if positive key is pressed, and 0 if both are pressed
float glfw_key_strength(GLFWwindow *p_win, int p_key, int n_key);

// allocates a buffer to the buffer referenced by "out_content", and sets the "out_size" to be content size.
// returns true if file loaded succesfully, false otherwise.
b8 try_load_file_text(const char *file_path, char **out_content, size_t *out_size);

// frees the buffer "out_content" returned by try_load_file_text
void file_text_free(char *content);


// X -> Y -> Z (parent to child relation, where Z is the most inner children)
void euler_radians_transform_xyz(vec3 euler_rads, mat4 applied_mat);

float clampf(float val, float min, float max);

#endif // _UTILS_H
