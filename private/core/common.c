#include <core/common.h>

void transform_apply(FlTransform transform, mat4 applied_mat) {
    glm_translate(applied_mat, transform.pos);

    euler_radians_transform_xyz(transform.rot, applied_mat);

    glm_scale(applied_mat, transform.scale);
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
