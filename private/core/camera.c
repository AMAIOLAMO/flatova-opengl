#include <core/camera.h>

#include <cglm/cglm.h>

void camera_forward(Camera cam, vec3 forward) {
    forward[0] = 0.0f;
    forward[1] = 0.0f;
    forward[2] = -1.0f;

    glm_vec3_rotate(forward, cam.v_rot, (vec3){1.0f, 0.0f, 0.0f});
    glm_vec3_rotate(forward, -cam.h_rot, (vec3){0.0f, 1.0f, 0.0f});
    glm_normalize(forward);
}

void camera_right(Camera cam, vec3 right) {
    vec3 forward;
    camera_forward(cam, forward);

    vec3 back;
    glm_vec3_negate_to(forward, back);

    glm_vec3_cross((vec3){ 0.0f, 1.0f, 0.0f }, back, right);
    glm_normalize(right);
}

void camera_up(Camera cam, vec3 up) {
    vec3 forward;
    camera_forward(cam, forward);

    vec3 back;
    glm_vec3_negate_to(forward, back);

    vec3 right;
    camera_right(cam, right);

    glm_vec3_cross(back, right, up);
    glm_normalize(up);
}

void camera_view_matrix(Camera cam, mat4 cam_mat) {
    vec3 forward;
    camera_forward(cam, forward);

    vec3 up;
    camera_up(cam, up);

    vec3 result;
    glm_vec3_add(cam.pos, forward, result);

    glm_lookat(cam.pos, result, up, cam_mat);
}

void camera_proj_matrix(Camera cam, int width, int height, mat4 proj_mat) {
    glm_perspective(cam.fov, (float)width / height, cam.near, cam.far, proj_mat);
}

