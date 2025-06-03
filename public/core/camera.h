#pragma once
#ifndef _FL_CORE_CAMERA_H
#define _FL_CORE_CAMERA_H

#include <cglm/cglm.h>

typedef struct Camera_t {
    vec3 pos;

    float h_rot;
    float v_rot;

    float fov;
    float near, far;
} Camera;

typedef struct CameraSettings_t {
    float max_vrot;
    float sensitivity;
    float speed_multiplier;
    float default_fly_speed;
} CameraSettings;

void camera_forward(Camera cam, vec3 forward);
void camera_right(Camera cam, vec3 right);
void camera_up(Camera cam, vec3 up);

void camera_view_matrix(Camera cam, mat4 cam_mat);
void camera_proj_matrix(Camera cam, int width, int height, mat4 proj_mat);

#endif // _FL_CORE_CAMERA_H
