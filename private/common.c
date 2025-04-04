#include <common.h>

void transform_apply(Transform transform, mat4 applied_mat) {
    glm_translate(applied_mat, transform.pos);

    euler_radians_transform_xyz(transform.rot, applied_mat);

    glm_scale(applied_mat, transform.scale);
}

Scene create_scene(const vec4 clear_color, FlEcsCtx *p_ctx) {
    return (Scene){
        .clear_color = {clear_color[0], clear_color[1], clear_color[2], clear_color[3]},
        .wireframe_mode = false,
        .p_ecs_ctx = p_ctx
    };
}

void scene_free(Scene *p_scene) {
    (void) p_scene;
}

