#include <core/common.h>
#include <string.h>

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
    assert(
        min <= max &&
        "cannot clamp when the minimum value is greater than the maximum"
    );
    if(val < min) return min;
    if(val > max) return max;

    return val;
}

b8 str_ends_with(const char *str, const char *cmp_str) {
    size_t str_len = strlen(str);
    size_t cmp_len = strlen(cmp_str);

    if(str_len < cmp_len)
        return false;
    // else

    return strncmp(str + (str_len - cmp_len), cmp_str, cmp_len) == 0;
}
