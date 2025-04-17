#include <common.h>

void transform_apply(FlTransform transform, mat4 applied_mat) {
    glm_translate(applied_mat, transform.pos);

    euler_radians_transform_xyz(transform.rot, applied_mat);

    glm_scale(applied_mat, transform.scale);
}

void scene_free(Scene *p_scene) {
    (void) p_scene;
}

