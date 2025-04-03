#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <cglm/cglm.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fast_obj.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include <nuklear.h>
#include <nuklear_glfw_gl3.h>

#include <nfd.h>

#include <cxlist.h>

#include <common.h>
#include <utils.h>
#include <camera.h>
#include <shader.h>
#include <model.h>
#include <primitives.h>

#include <editor.h>
#include <fl_ecs.h>

#include <resources.h>
#include <primitive_resources.h>

// place this after GLFW window initialization, since it requires glfwGetProcAddress
// and glfw is only initialized when the window has been created
b8 init_glad(void) {
    return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void framebuffer_size_callback(GLFWwindow* p_win, int width, int height) {
    (void) p_win;
    glViewport(0, 0, width, height);
    printf("Detected frame buffer size change\n");
}

typedef struct Pipeline_t {
    GLuint vert_buf;
    GLuint norm_buf;
    GLuint tex_coord_buf;
    GLuint elem_buf;

    GLuint vert_arr;
} Pipeline;

Camera cam = {
    .pos  = { 0.0f, 0.5f, 3.0f },

    .h_rot = 0.0f,
    .v_rot = 0.0f,

    .fov  = 60.0f * DEG2RAD,
    .near = 0.1f,
    .far  = 100.0f
};

CameraSettings cam_settings = {
    .max_vrot          = 89.0f * DEG2RAD,
    .sensitivity       = 0.0007f,
    .speed_multiplier  = 1.0f,
    .default_fly_speed = 2.0f
};

void glfw_cursor_callback(GLFWwindow *p_win, double x, double y) {
    static b8 cursor_pressed = false;

    static double last_x;
    static double last_y;

    if(glfwGetMouseButton(p_win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if(cursor_pressed == false) {
            cursor_pressed = true;
            last_x = x;
            last_y = y;
            glfwSetInputMode(p_win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            return;
        }

        double offset_x = x - last_x;
        double offset_y = y - last_y;

        last_x = x;
        last_y = y;

        cam.h_rot += offset_x * cam_settings.sensitivity;
        cam.v_rot += -offset_y * cam_settings.sensitivity;

        if(cam.v_rot > cam_settings.max_vrot)
            cam.v_rot = cam_settings.max_vrot;

        if(cam.v_rot < -cam_settings.max_vrot)
            cam.v_rot = -cam_settings.max_vrot;

        return;
    }
    // else

    if(cursor_pressed) {
        glfwSetInputMode(p_win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        cursor_pressed = false;
    }
}

void process_input(GLFWwindow *p_win, float dt) {
    if(glfwGetKey(p_win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(p_win, true);

    float cam_move_speed = cam_settings.default_fly_speed * cam_settings.speed_multiplier;

    if(glfwGetKey(p_win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cam_move_speed *= 2;

    vec3 forward;
    camera_forward(cam, forward);
    glm_vec3_scale(
        forward, glfw_key_strength(p_win, GLFW_KEY_W, GLFW_KEY_S) * cam_move_speed * dt,
        forward
    );

    glm_vec3_add(cam.pos, forward, cam.pos);

    vec3 right;
    camera_right(cam, right);
    glm_vec3_scale(
        right, glfw_key_strength(p_win, GLFW_KEY_D, GLFW_KEY_A) * cam_move_speed * dt,
        right 
    );

    glm_vec3_add(cam.pos, right, cam.pos);

    cam.pos[1] += glfw_key_strength(p_win, GLFW_KEY_E, GLFW_KEY_Q) * cam_move_speed * dt;
}

void render_grid(Resources resources, const Pipeline pipeline, Camera *p_cam, mat4 view_proj_mat) {
    Shader *p_grid = (Shader*)resources_find(resources, "shaders/grid");

    shader_use(*p_grid); {
        vertex_bind_load_buffers(
            get_prim_plane_vertices(), get_prim_plane_vertices_size(),
            pipeline.vert_buf, pipeline.norm_buf, pipeline.tex_coord_buf
        );

        shader_set_uniform_mat4fv(*p_grid, "view_proj", view_proj_mat);

        mat4 local_to_world_mat;
        glm_mat4_identity(local_to_world_mat);
        glm_translate(local_to_world_mat, (vec3){p_cam->pos[0], 0.0f, p_cam->pos[2]});

        shader_set_uniform_mat4fv(*p_grid, "model", local_to_world_mat);

        glDrawArrays(GL_TRIANGLES, 0, get_prim_plane_vertices_size());
    }
}


void render_skybox(Resources resources, const Pipeline pipeline, Camera *p_cam, mat4 view_proj_mat) {
    Shader *p_skybox_shader = (Shader*)resources_find(resources, "shaders/skybox");
    Model *p_skybox_model = (Model*)resources_find(resources, "primitives/skybox");
    GLuint *p_skybox_tex = (GLuint*)resources_find(resources, "textures/skymap");

    shader_use(*p_skybox_shader); {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *p_skybox_tex);

        vertex_bind_load_buffers(
            p_skybox_model->verts, p_skybox_model->verts_count,
            pipeline.vert_buf, pipeline.norm_buf, pipeline.tex_coord_buf
        );

        shader_set_uniform_1i(*p_skybox_shader, "skymap", 0);

        shader_set_uniform_mat4fv(*p_skybox_shader, "view_proj", view_proj_mat);

        mat4 local_to_world_mat;
        glm_mat4_identity(local_to_world_mat);
        // TODO: we should not do this, the perspective calculation should be handled separately
        // where near plane should not affect the skybox
        glm_translate(local_to_world_mat, p_cam->pos);
        glm_scale(local_to_world_mat, (vec3){p_cam->far, p_cam->far, p_cam->far});

        shader_set_uniform_mat4fv(*p_skybox_shader, "model", local_to_world_mat);

        glDrawArrays(GL_TRIANGLES, 0, p_skybox_model->verts_count);
    }
}

void render_scene_frame(GLFWwindow *p_win, const Pipeline pipeline, Scene *p_scene,
                        Resources resources, FlComponent mesh_render_comp, FlComponent transform_comp) {
    glClearColor(
        p_scene->clear_color[0], p_scene->clear_color[1],
        p_scene->clear_color[2], p_scene->clear_color[3]
    );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int width, height;
    glfwGetWindowSize(p_win, &width, &height);

    mat4 cam_mat;
    camera_view_matrix(cam, cam_mat);

    mat4 proj_mat;
    camera_proj_matrix(cam, width, height, proj_mat);

    mat4 view_proj_mat;
    glm_mat4_mul(proj_mat, cam_mat, view_proj_mat);

    glBindVertexArray(pipeline.vert_arr);
    render_skybox(resources, pipeline, &cam, view_proj_mat);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    FlEcsCtx *p_ecs_ctx = p_scene->p_ecs_ctx;

    size_t iter = 0;
    FlEntity entity;

    FlComponent query_components[] = {
        transform_comp, mesh_render_comp
    };

    while(fl_ecs_query(p_ecs_ctx, &iter, &entity, query_components, arr_size(query_components))) {
        MeshRender *p_mesh_render = fl_ecs_get_entity_component_data(p_ecs_ctx, entity, mesh_render_comp);
        Transform *p_transform    = fl_ecs_get_entity_component_data(p_ecs_ctx, entity, transform_comp);

        assert(p_mesh_render);
        assert(p_transform);

        const Shader *p_shader = p_mesh_render->p_shader;
        const Model *p_model   = p_mesh_render->p_model;


        vertex_bind_load_buffers(
            p_model->verts,
            p_model->verts_count,
            pipeline.vert_buf, pipeline.norm_buf, pipeline.tex_coord_buf
        );

        shader_use(*p_shader); {
            shader_set_uniform_3f(*p_shader, "light_pos", p_scene->light_pos);
            shader_set_uniform_3f(*p_shader, "albedo_color", p_mesh_render->albedo_color);
            shader_set_uniform_3f(*p_shader, "cam_pos", cam.pos);
            shader_set_uniform_mat4fv(*p_shader, "view_proj", view_proj_mat);

            mat4 local_to_world_mat;
            glm_mat4_identity(local_to_world_mat);
            transform_apply(*p_transform, local_to_world_mat);

            shader_set_uniform_mat4fv(*p_shader, "model", local_to_world_mat);

            mat4 normal_mat;
            glm_mat4_identity(normal_mat);
            euler_radians_transform_xyz(p_transform->rot, normal_mat);

            shader_set_uniform_mat4fv(*p_shader, "norm_mat", normal_mat);
        }

        if(p_scene->wireframe_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(pipeline.vert_arr);
        glDrawArrays(GL_TRIANGLES, 0, p_model->verts_count);

        if(p_scene->wireframe_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDisable(GL_CULL_FACE);

    glBindVertexArray(pipeline.vert_arr);
    render_grid(resources, pipeline, &cam, view_proj_mat);

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}

FlScaling __g_scaling_handle = {{1.0f, 1.0f}};

void g_scaling_set_dpi(float xscale, float yscale) {
    scaling_set_dpi(&__g_scaling_handle, xscale, yscale);
}

#define DPI_SCALEX(VALUE) ((VALUE) * __g_scaling_handle.dpi_scale[0])
#define DPI_SCALEY(VALUE) ((VALUE) * __g_scaling_handle.dpi_scale[1])

const nk_flags DEFAULT_NK_WIN_FLAGS = NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
    NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE;

const nk_flags EMPTY_NK_WIN_FLAGS = 0;

/*typedef struct FlUICtx_t {*/
/*    struct nk_context *p_nk_ctx;*/
/*    FlScaling scaling;*/
/*} FlUICtx;*/

void fl_xyz_widget(struct nk_context *p_ctx, const char *name, vec3 p_pos, float min, float max) {
    assert(min <= max && "the min value should always be smaller or equal to the max value");
    nk_flags group_flags = NK_WINDOW_BORDER | NK_WINDOW_TITLE;

    nk_layout_row_dynamic(p_ctx, DPI_SCALEY(50), 1);

    if (nk_group_begin(p_ctx, name, group_flags)) {
        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(18), 3);
        nk_property_float(p_ctx, "#x:", min, &p_pos[0], max, 0.1f, 0.2f);
        nk_property_float(p_ctx, "#y:", min, &p_pos[1], max, 0.1f, 0.2f);
        nk_property_float(p_ctx, "#z:", min, &p_pos[2], max, 0.1f, 0.2f);

        nk_group_end(p_ctx);
    }
}

const float HUGE_VALUEF = 9999999999.0f;

void render_combo_color_picker(struct nk_context *p_ctx, struct nk_colorf *p_color) {
    if (nk_combo_begin_color(
        p_ctx, nk_rgb_cf(*p_color),
        nk_vec2(DPI_SCALEX(200), DPI_SCALEY(400))
    )) {
        enum color_mode {COL_RGB, COL_HSV};

        static int col_mode = COL_RGB;
        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(120), 1);
        *p_color = nk_color_picker(p_ctx, *p_color, NK_RGBA);

        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 2);
        col_mode = nk_option_label(p_ctx, "RGB", col_mode == COL_RGB) ? COL_RGB : col_mode;
        col_mode = nk_option_label(p_ctx, "HSV", col_mode == COL_HSV) ? COL_HSV : col_mode;

        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 1);

        if (col_mode == COL_RGB) {
            p_color->r = nk_propertyf(p_ctx, "#R:", 0, p_color->r, 1.0f, 0.01f,0.005f);
            p_color->g = nk_propertyf(p_ctx, "#G:", 0, p_color->g, 1.0f, 0.01f,0.005f);
            p_color->b = nk_propertyf(p_ctx, "#B:", 0, p_color->b, 1.0f, 0.01f,0.005f);
            p_color->a = nk_propertyf(p_ctx, "#A:", 0, p_color->a, 1.0f, 0.01f,0.005f);
        } else {
            float hsva[4];
            nk_colorf_hsva_fv(hsva, *p_color);

            hsva[0] = nk_propertyf(p_ctx, "#H:", 0, hsva[0], 1.0f, 0.01f,0.05f);
            hsva[1] = nk_propertyf(p_ctx, "#S:", 0, hsva[1], 1.0f, 0.01f,0.05f);
            hsva[2] = nk_propertyf(p_ctx, "#V:", 0, hsva[2], 1.0f, 0.01f,0.05f);
            hsva[3] = nk_propertyf(p_ctx, "#A:", 0, hsva[3], 1.0f, 0.01f,0.05f);

            *p_color = nk_hsva_colorfv(hsva);
        }
        nk_combo_end(p_ctx);
    }
}

// hierarchy
void render_scene_hierarchy(struct nk_context *p_ctx, Scene *p_scene, FlEditorComponents *p_comps,
                            Resources resources, FlEntity *p_chosen_entity) {
    if (nk_begin(p_ctx, "Scene Hierarchy", nk_rect(DPI_SCALEX(20), DPI_SCALEY(50),
                                                    DPI_SCALEX(340), DPI_SCALEY(250)), DEFAULT_NK_WIN_FLAGS)) {
        const Model *p_model = resources_find(resources, "primitives/default_model");

        nk_menubar_begin(p_ctx);

        nk_layout_row_begin(p_ctx, NK_STATIC, DPI_SCALEY(25), 3);
        nk_layout_row_push(p_ctx, DPI_SCALEX(25));

        FlEcsCtx *p_ecs_ctx = p_scene->p_ecs_ctx;

        const FlComponent transform_id = p_comps->transform;
        const FlComponent mesh_render_id = p_comps->mesh_render;

        if(nk_button_label(p_ctx, "+")) {
            // TODO: the UI should not have the power to modify entities directly
            const FlEntity entity = fl_ecs_entity_add(p_ecs_ctx);

            Transform *p_transform = fl_ecs_get_entity_component_data(p_ecs_ctx, entity, transform_id);
            *p_transform = (Transform){.pos = {0.0f, 0.0f, 0.0f}, .scale = {1.0f, 1.0f, 1.0f}};

            MeshRender *p_render = fl_ecs_get_entity_component_data(p_ecs_ctx, entity, mesh_render_id);

            fl_ecs_entity_activate_component(p_ecs_ctx, entity, transform_id, true);
            fl_ecs_entity_activate_component(p_ecs_ctx, entity, mesh_render_id, true);

            Shader *p_phong = (Shader*)resources_find(resources, "shaders/phong");

            *p_render = (MeshRender){ .p_model = p_model, .p_shader = p_phong, .albedo_color = {1.0f, 1.0f, 1.0f} };
        }

        nk_layout_row_push(p_ctx, DPI_SCALEX(120));

        nk_labelf(p_ctx, NK_TEXT_CENTERED, "Entity Count: %zu / %zu", p_ecs_ctx->entity_count, p_ecs_ctx->entity_cap);
        nk_labelf(p_ctx, NK_TEXT_CENTERED, "Entity Global Id: %zu", p_ecs_ctx->g_entity_id);

        nk_menubar_end(p_ctx);
        nk_layout_row_end(p_ctx);


        
        size_t iter = 0;
        FlEntity entity;

        FlComponent query_components[] = {
            transform_id
        };


        // TODO: instead of checking specifically components, we just loop and allowing choosing of entities
        if (nk_tree_push(p_ctx, NK_TREE_NODE, "Entities", NK_MINIMIZED)) {

            char txt_buf[256] = {0};
            while(fl_ecs_query(p_ecs_ctx, &iter, &entity, query_components, arr_size(query_components))) {
                nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 1);
                snprintf(txt_buf, arr_size(txt_buf), "[%zu] Entity", entity);

                nk_bool is_chosen = *p_chosen_entity == entity;

                if(nk_selectable_label(p_ctx, txt_buf, NK_TEXT_LEFT, &is_chosen))
                    *p_chosen_entity = entity;
            }

            nk_tree_pop(p_ctx);
        }
    }
    nk_end(p_ctx);
}

void render_camera_properties(struct nk_context *p_ctx, GLFWwindow *p_win) {
    int width, height;
    glfwGetWindowSize(p_win, &width, &height);

    // camera properties
    if (nk_begin(p_ctx, "Camera Properties", nk_rect(width - DPI_SCALEX(340 + 20), DPI_SCALEY(50),
                                                      DPI_SCALEX(340), DPI_SCALEY(250)), DEFAULT_NK_WIN_FLAGS)) {
        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(18), 1);
        fl_xyz_widget(p_ctx, "Position", cam.pos, -HUGE_VALUEF, HUGE_VALUEF);

        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 2);
        nk_property_float(p_ctx, "h(rad):", -FL_TAU, &cam.h_rot, FL_TAU, 0.1f, 0.01f);
        nk_property_float(p_ctx, "v(rad):", glm_rad(-89.0f), &cam.v_rot, glm_rad(89.0f), 0.1f, 0.01f);

        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 1);
        nk_property_float(p_ctx, "Field of view(rad):", 0.01f, &cam.fov, FL_PI, 0.1f, 0.01f);
        nk_property_float(p_ctx, "near plane(meter):", 0.01f, &cam.near, HUGE_VALUEF, 0.1f, 0.01f);
        nk_property_float(p_ctx, "far plane(meter):", 0.01f,  &cam.far,  HUGE_VALUEF, 0.1f, 0.01f);

        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 1);
        nk_property_float(p_ctx, "speed multiplier:", 0.01f, &cam_settings.speed_multiplier, HUGE_VALUEF, 0.1f, 0.01f);
    }
    nk_end(p_ctx);
}

void render_editor_metrics(struct nk_context *p_ctx, GLFWwindow *p_win, float dt) {
    int width, height;
    glfwGetWindowSize(p_win, &width, &height);

    if (nk_begin(p_ctx, "Editor Metrics", nk_rect(width - DPI_SCALEX(200 + 20), DPI_SCALEY(50),
                                                   DPI_SCALEX(200), DPI_SCALEY(50)), EMPTY_NK_WIN_FLAGS)) {
        float fps = 1.0f / dt;

        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(15), 1);
        nk_label(p_ctx, "Editor Metrics", NK_TEXT_CENTERED);
        nk_labelf(p_ctx, NK_TEXT_CENTERED, "frames per second(FPS): %.2fs", fps);
    }
    nk_end(p_ctx);
}

void resources_model_free(void *p_raw) {
    model_free(p_raw);
}

void render_resource_viewer(struct nk_context *p_ctx, Resources resources) {
    if (nk_begin(p_ctx, "Resource Viewer", nk_rect(DPI_SCALEX(20), DPI_SCALEY(400),
                                                    DPI_SCALEX(400), DPI_SCALEY(250)), DEFAULT_NK_WIN_FLAGS)) {
        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 1);
        if(nk_button_label(p_ctx, "load model")) {

            nfdu8char_t *model_path;
            nfdu8filteritem_t filters[] = { { "Wavefront Obj", "obj" } };

            nfdopendialogu8args_t args = {0};
            args.filterList = filters;
            args.filterCount = arr_size(filters);

            nfdresult_t result = NFD_OpenDialogU8_With(&model_path, &args);

            if (result == NFD_OKAY) {
                puts("Info: Found model at path\n");
                puts(model_path);
                
                // TODO: automate this, make a simple function to do this instead
                Model *p_model = load_model_obj(model_path);
                resources_store_auto(resources, "primitives/a", p_model, resources_model_free);

                NFD_FreePathU8(model_path);
            }
            else if (result == NFD_CANCEL) {
                puts("Info: pressed cancel.");
            }
            else {
                printf("Error: %s\n", NFD_GetError());
            }
        }
        
        size_t iter = 0;
        Resource *p_resource = NULL;

        size_t idx = 0;

        while(resources_iter(resources, &iter, &p_resource)) {
            nk_layout_row_dynamic(p_ctx, DPI_SCALEY(20), 2);
            nk_labelf(p_ctx, NK_TEXT_LEFT, "[%zu] %s: ", idx, p_resource->identifier);
            nk_labelf(p_ctx, NK_TEXT_RIGHT, "%p", p_resource->p_raw);

            idx += 1;
        }
    }
    nk_end(p_ctx);
}

void render_scene_settings(struct nk_context *p_ctx, Scene *p_scene) {
    if (nk_begin(p_ctx, "Scene Settings", nk_rect(DPI_SCALEX(20), DPI_SCALEY(600),
                                                    DPI_SCALEX(400), DPI_SCALEY(250)), DEFAULT_NK_WIN_FLAGS)) {
        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 2);

        nk_bool nk_wireframe = p_scene->wireframe_mode;
        nk_checkbox_label(p_ctx, "wireframe", &nk_wireframe);
        p_scene->wireframe_mode = nk_wireframe;

        struct nk_colorf nk_obj_color = {
            p_scene->clear_color[0], p_scene->clear_color[1],
            p_scene->clear_color[2], p_scene->clear_color[3]
        };

        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 1);
        render_combo_color_picker(p_ctx, &nk_obj_color);

        p_scene->clear_color[0] = nk_obj_color.r;
        p_scene->clear_color[1] = nk_obj_color.g;
        p_scene->clear_color[2] = nk_obj_color.b;
        p_scene->clear_color[3] = nk_obj_color.a;
    }
    nk_end(p_ctx);
}


void render_main_menubar(struct nk_context *p_ctx, GLFWwindow *p_win, FlEditorCtx *p_editor_ctx) {
    int width, height;
    glfwGetWindowSize(p_win, &width, &height);

    if (nk_begin(p_ctx, "Main Menubar", nk_rect(0, 0, width, DPI_SCALEY(30)), EMPTY_NK_WIN_FLAGS)) {
        nk_menubar_begin(p_ctx);

        nk_layout_row_begin(p_ctx, NK_STATIC, DPI_SCALEY(25), 1);
        nk_layout_row_push(p_ctx, DPI_SCALEY(45));

        if (nk_menu_begin_label(
            p_ctx, "window", NK_TEXT_LEFT,
            nk_vec2(DPI_SCALEX(140), DPI_SCALEY(200))
        )) {
            nk_layout_row_dynamic(p_ctx, DPI_SCALEY(20), 1);

            size_t iter = 0;
            FlWidgetCtx *p_widget_ctx = NULL;

            while(editor_ctx_iter(*p_editor_ctx, &iter, &p_widget_ctx)) {
                nk_bool is_open = p_widget_ctx->is_open;
                nk_checkbox_label(p_ctx, p_widget_ctx->identifier, &is_open);
                editor_ctx_set_widget_open(*p_editor_ctx, p_widget_ctx->identifier, is_open);
            }

            nk_menu_end(p_ctx);
        }

        nk_menubar_end(p_ctx);
        nk_layout_row_end(p_ctx);
    }
    nk_end(p_ctx);
}

void render_file_browser(struct nk_context *p_ctx) {
    if (nk_begin(p_ctx, "File Browser",
                 nk_rect(DPI_SCALEX(20), DPI_SCALEY(50), DPI_SCALEX(300), DPI_SCALEY(150)),
                 DEFAULT_NK_WIN_FLAGS)) {
    }
    nk_end(p_ctx);
}

void render_entity_inspector(struct nk_context *p_ctx, Scene *p_scene, Resources resources,
                             FlEditorComponents *p_comps, FlEntity *p_chosen_entity) {
    if (nk_begin(p_ctx, "Entity Inspector",
                 nk_rect(DPI_SCALEX(20), DPI_SCALEY(500), DPI_SCALEX(300), DPI_SCALEY(400)),
                 DEFAULT_NK_WIN_FLAGS)) {
        FlEcsCtx *p_ecs_ctx = p_scene->p_ecs_ctx;


        // TODO: HACK entity checking is not strict here, it will break when we introduce deleting of entities
        // since we will move around the entity IDs in memory, so FlEntity cannot be directly used to check whether or not
        // it is a valid entity
        if(p_ecs_ctx->entity_count > 0 && (*p_chosen_entity) < p_ecs_ctx->entity_count) {
            const FlComponent transform_id = p_comps->transform;

            // TODO: this should be separated, each component should handle their own rendering, instead of being rendered here
            if(fl_ecs_entity_has_component(p_ecs_ctx, *p_chosen_entity, transform_id)) {
                Transform *p_transform = fl_ecs_get_entity_component_data(p_ecs_ctx, *p_chosen_entity, transform_id);
                
                nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 1);
                fl_xyz_widget(p_ctx, "Position", p_transform->pos, -HUGE_VALUEF, HUGE_VALUEF);

                fl_xyz_widget(p_ctx, "Rotation", p_transform->rot, -FL_TAU, FL_TAU);
                fl_xyz_widget(p_ctx, "Scale", p_transform->scale, -HUGE_VALUEF, HUGE_VALUEF);
            }

            const FlComponent mesh_render_id = p_comps->mesh_render;
            if(fl_ecs_entity_has_component(p_ecs_ctx, *p_chosen_entity, mesh_render_id)) {
                MeshRender *p_mesh_render = fl_ecs_get_entity_component_data(p_ecs_ctx, *p_chosen_entity, mesh_render_id);

                size_t r_iter = 0;
                Resource *p_resource = NULL;

                const char *models[50] = {0};
                size_t items_size = 0;

                while(resources_iter(resources, &r_iter, &p_resource)) {
                    if(strncmp(p_resource->identifier, "primitives", strlen("primitives")) == 0)
                        models[items_size++] = p_resource->identifier;
                }

                static int selected = 0;

                nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 1);
                selected = nk_combo(p_ctx, models, items_size, selected, DPI_SCALEY(15), nk_vec2(DPI_SCALEX(100), DPI_SCALEY(200)));

                Model *p_chose_model = (Model*)resources_find(resources, models[selected]);

                p_mesh_render->p_model = p_chose_model;

                vec3 *p_color = &p_mesh_render->albedo_color;

                struct nk_colorf nk_obj_color = { (*p_color)[0], (*p_color)[1], (*p_color)[2], 1.0f };

                nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 1);
                render_combo_color_picker(p_ctx, &nk_obj_color);

                (*p_color)[0] = nk_obj_color.r;
                (*p_color)[1] = nk_obj_color.g;
                (*p_color)[2] = nk_obj_color.b;
            }
        }
    }
    nk_end(p_ctx);
}

int fl_glfw_init(GLFWwindow **pp_win) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *p_win = glfwCreateWindow(1000, 900, "Flatova", NULL, NULL);
    *pp_win = p_win;

    if(p_win == NULL) {
        printf("Error: failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(p_win);

    if(!init_glad()) {
        printf("Error: Failed to initialize GLAD\n");
        glfwTerminate();
        return false;
    }

    // GLFW CALLBACKS
    glfwSetFramebufferSizeCallback(p_win, framebuffer_size_callback);

    GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
    float dpi_xscale, dpi_yscale;
    glfwGetMonitorContentScale(primary_monitor, &dpi_xscale, &dpi_yscale);

    printf("Found primary monitor scale: <%f, %f>\n", dpi_xscale, dpi_yscale);

    g_scaling_set_dpi(dpi_xscale, dpi_yscale);
    
    return true;
}

int main(void) {
    const size_t TBL_ENTITY_COUNT = 100;
    const size_t TBL_COMPONENT_COUNT = 32;

    FlEcsCtx ecs_ctx = fl_ecs_ctx_create(TBL_ENTITY_COUNT, TBL_COMPONENT_COUNT);

    const FlComponent TRANSFORM_ID = fl_ecs_add_component(
        &ecs_ctx, sizeof(Transform)
    );

    const FlComponent MESH_RENDER_ID = fl_ecs_add_component(
        &ecs_ctx, sizeof(MeshRender)
    );

    NFD_Init();

    Resources resources = resources_create();

    char cwd_path[512];
    getcwd(cwd_path, arr_size(cwd_path));
    printf("running on working directory: %s\n", cwd_path);

    Model *default_model = load_model_obj("vendor/primitive_models/test_complex.obj");
    resources_store(resources, "primitives/default_model", default_model);

    Model *skybox = load_model_obj("vendor/primitive_models/skybox.obj");
    resources_store(resources, "primitives/skybox", skybox);

    GLFWwindow *p_win = NULL;

    if(fl_glfw_init(&p_win) == false) {
        printf("Error: Failed to initialize GLFW! aborting...\n");
        return -1;
    }

    // NUKLEAR SETUP
    struct nk_glfw nk_glfw = {0};
    struct nk_context *nk_ctx;

    nk_ctx = nk_glfw3_init(&nk_glfw, p_win, NK_GLFW3_INSTALL_CALLBACKS);

    {
        struct nk_font_atlas *nk_atlas;

        const int DEFAULT_FONT_SIZE = 16;
        const int DPI_SCALED_FONT_SIZE = DPI_SCALEX(DEFAULT_FONT_SIZE);

        nk_glfw3_font_stash_begin(&nk_glfw, &nk_atlas);
        struct nk_font *roboto_regular = nk_font_atlas_add_from_file(
            nk_atlas,
            "vendor/default_fonts/Roboto-Regular.ttf", DPI_SCALED_FONT_SIZE, 0
        );
        nk_glfw3_font_stash_end(&nk_glfw);
        nk_style_set_font(nk_ctx, &roboto_regular->handle);

        resources_store(resources, "fonts/roboto_regular", roboto_regular);
    }


    // LOAD all shaders in vendor
    if( resources_load_shader_from_files(
        resources, "shaders/grid",
        "./shaders/grid.vs", "./shaders/grid.fs"
    ) == NULL) {
        printf("Error: Failed to load grid shaders!\n");
        glfwTerminate();
        return -1;
    }

    printf("Grid Shaders loaded\n");


    Shader *p_unlit_texture_shader = resources_load_shader_from_files(
        resources, "shaders/unlit",
        "./shaders/unlit.vs", "./shaders/unlit.fs"
    );

    if(p_unlit_texture_shader == NULL) {
        printf("Error: Failed to load unlit shaders!\n");
        glfwTerminate();
        return -1;
    }

    printf("Unlit Shaders loaded\n");

    Shader *p_phong_shader = resources_load_shader_from_files(
        resources, "shaders/phong",
        "./shaders/phong.vs", "./shaders/phong.fs"
    );

    if(p_phong_shader == NULL) {
        printf("Error: Failed to load phong shaders!\n");
        glfwTerminate();
        return -1;
    }

    printf("Phong shader loaded\n");

    Shader *p_skybox_shader = resources_load_shader_from_files(
        resources, "shaders/skybox",
        "./shaders/skybox.vs", "./shaders/skybox.fs"
    );

    if(p_skybox_shader == NULL) {
        printf("Error: Failed to load skybox shaders!\n");
        glfwTerminate();
        return -1;
    }

    printf("Skybox shader loaded\n");

    resources_store(resources, "primitives/plane", get_prim_plane_vertices());

    GLuint skymap_tex;
    if(gl_try_load_texture2d_linear("vendor/skymaps/skymap_1.png", &skymap_tex, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE) == false) {
        printf("Error: Failed to load skybox texture!\n");
        glfwTerminate();
        return -1;
    }
    resources_store(resources, "textures/skymap", &skymap_tex);

    printf("Skybox texture loaded\n");

    // create buffers
    GLuint vert_arr_obj;
    glGenVertexArrays(1, &vert_arr_obj);

    glBindVertexArray(vert_arr_obj);


    GLuint vert_buf_obj;
    glGenBuffers(1, &vert_buf_obj);

    glBindBuffer(GL_ARRAY_BUFFER, vert_buf_obj);
    glVertexAttribPointer(0, sizeof(vec3) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));

    GLuint tex_coord_buf_obj;
    glGenBuffers(1, &tex_coord_buf_obj);

    glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buf_obj);
    glVertexAttribPointer(1, sizeof(vec2) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));

    GLuint normal_buf_obj;
    glGenBuffers(1, &normal_buf_obj);

    glBindBuffer(GL_ARRAY_BUFFER, normal_buf_obj);
    glVertexAttribPointer(2, sizeof(vec3) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    GLuint elem_buf_obj;
    glGenBuffers(1, &elem_buf_obj);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buf_obj);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    Pipeline pipe = {
        vert_buf_obj,
        tex_coord_buf_obj,
        normal_buf_obj,
        elem_buf_obj,

        vert_arr_obj
    };

    glfwSetCursorPosCallback(p_win, glfw_cursor_callback);

    GLuint scene_frame_buffer;
    glGenFramebuffers(1, &scene_frame_buffer);

    float dt = glfwGetTime();
    float prev_time = glfwGetTime();

    Scene scene = create_scene((vec4){0.0f, 0.0f, 0.0f, 1.0f}, &ecs_ctx);

    FlEditorCtx editor_ctx = create_editor_ctx();
    editor_ctx_register_widget(editor_ctx, "scene hierarchy");
    editor_ctx_register_widget(editor_ctx, "camera properties");
    editor_ctx_register_widget(editor_ctx, "resource viewer");
    editor_ctx_register_widget(editor_ctx, "scene settings");
    editor_ctx_register_widget(editor_ctx, "file browser");
    editor_ctx_register_widget(editor_ctx, "entity inspector");

    while(!glfwWindowShouldClose(p_win)) {
        float current_time = glfwGetTime();
        dt = current_time - prev_time;
        prev_time = current_time;

        process_input(p_win, dt);

        nk_glfw3_new_frame(&nk_glfw);

        render_main_menubar(nk_ctx, p_win, &editor_ctx);

        FlEditorComponents comps = {
            .transform = TRANSFORM_ID,
            .mesh_render = MESH_RENDER_ID
        };

        static FlEntity chosen_entity = 0;

        if(editor_ctx_is_widget_open(editor_ctx, "scene hierarchy"))
            render_scene_hierarchy(nk_ctx, &scene, &comps, resources, &chosen_entity);

        if(editor_ctx_is_widget_open(editor_ctx, "camera properties"))
            render_camera_properties(nk_ctx, p_win);

        if(editor_ctx_is_widget_open(editor_ctx, "resource viewer"))
            render_resource_viewer(nk_ctx, resources);

        if(editor_ctx_is_widget_open(editor_ctx, "scene settings"))
            render_scene_settings(nk_ctx, &scene);

        if(editor_ctx_is_widget_open(editor_ctx, "file browser"))
            render_file_browser(nk_ctx);

        if(editor_ctx_is_widget_open(editor_ctx, "entity inspector"))
            render_entity_inspector(nk_ctx, &scene, resources, &comps, &chosen_entity);

        render_editor_metrics(nk_ctx, p_win, dt);


        // RENDERING
        render_scene_frame(p_win, pipe, &scene, resources, MESH_RENDER_ID, TRANSFORM_ID);

        const size_t NK_MAX_VERTEX_BUFFER  = 512 * 1024;
        const size_t NK_MAX_ELEMENT_BUFFER = 128 * 1024;

        nk_glfw3_render(&nk_glfw, NK_ANTI_ALIASING_ON, NK_MAX_VERTEX_BUFFER, NK_MAX_ELEMENT_BUFFER);

        glfwSwapBuffers(p_win);
        glfwPollEvents();
    }

    editor_ctx_free(editor_ctx);
    fl_ecs_ctx_free(&ecs_ctx);

    model_free(default_model);
    model_free(skybox);

    scene_free(&scene);
    resources_free(resources);
    NFD_Quit();

    nk_glfw3_shutdown(&nk_glfw);
    glfwTerminate();

    return 0;
}
