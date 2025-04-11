#include "cxlist.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <cglm/cglm.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
#include <tinydir.h>

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
    Model *p_skybox_model   = (Model*)resources_find(resources, "primitives/skybox");
    GLuint *p_skybox_tex    = (GLuint*)resources_find(resources, "textures/skymap");

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

            shader_set_uniform_3f(*p_shader, "material.diffuse", p_mesh_render->albedo_color);
            shader_set_uniform_3f(*p_shader, "material.specular", p_mesh_render->specular_color);
            shader_set_uniform_1f(*p_shader, "material.specular_factor", p_mesh_render->specular_factor);

            shader_set_uniform_3f(*p_shader, "material.ambient", p_scene->ambient_color);

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

// checks whether or not a directory is a special navigation(. and ..)
int tinydir_is_nav(const tinydir_file file) {
    return file.is_dir && (
        strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0
    );
}

typedef struct ResIdStrings_t {
    size_t len, cap;
    char **data;
} ResIdStrings;

void resources_unload_idstrings(ResIdStrings *p_strs) {
    for(size_t i = 0; i < p_strs->len; i++) {
        free(p_strs->data[i]);
    }
    list_free(p_strs);
}

// TODO: maybe allocate this alloc strings inside of resources itself?
void resources_load_dir_recursive(Resources res, size_t depth, const char *path, ResIdStrings *p_alloc_strings) {
    tinydir_dir vendor_dir = {0};

    if(tinydir_open(&vendor_dir, path) == -1) {
        perror("ERROR: cannot open vendor directory!");
    }
    else {
        while(vendor_dir.has_next) {
            tinydir_file file;
            if(tinydir_readfile(&vendor_dir, &file) == -1)
                continue;

            if(file.is_dir) {
                // normal directories (not . and ..)
                if(!tinydir_is_nav(file)) {
                    if(depth > 0)
                        resources_load_dir_recursive(res, depth - 1, file.path, p_alloc_strings);
                }
            }

            const char *PRIMITIVES_PREFIX = "primitives/";
            /*const char *SHADER_PREFIX     = "shaders/";*/
            /*const char *TEXTURE_PREFIX    = "textures/";*/

            if(file.is_reg) {
                if(strcmp(file.extension, "obj") == 0) {
                    size_t str_len = strlen(PRIMITIVES_PREFIX) + strlen(file.name) - strlen(file.extension) + 1;
                    char *id = malloc(str_len);

                    memset(id, '\0', str_len);
                    strncpy(id, PRIMITIVES_PREFIX, strlen(PRIMITIVES_PREFIX));
                    strncpy(id + strlen(PRIMITIVES_PREFIX), file.name, strlen(file.name) - strlen(file.extension) - 1);

                    list_append(p_alloc_strings, id);

                    resources_load_model_from_obj(res, id, file.path);
                    printf("OBJ: %s -> %s ext: %s", file.name, file.path, file.extension);
                }
            }


            printf("\n");

            if(tinydir_next(&vendor_dir) == -1)
                perror("ERROR; cannot get next file");

        }
    }

    tinydir_close(&vendor_dir);
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

    // loop through contents in vendor
    const size_t DIR_DEPTH = 5;
    ResIdStrings load_strings = {0};
    resources_load_dir_recursive(resources, DIR_DEPTH, TINYDIR_STRING("./vendor"), &load_strings);

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
        const int DPI_SCALED_FONT_SIZE = g_scaling_x(DEFAULT_FONT_SIZE);

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
    if(resources_load_shader_from_files(
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

    /*resources_load_texture2d_linear_from_file("vendor/skymaps/skymap_1.png", GL_RGB, GL_UNSIGNED_BYTE);*/

    GLuint skymap_tex;
    if(gl_try_load_texture2d_linear("vendor/skymaps/skymap_1.png", &skymap_tex, GL_RGB) == false) {
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

    float dt = glfwGetTime();
    float prev_time = glfwGetTime();

    Scene scene = {
        .clear_color = {0.0f, 0.0f, 0.0f, 1.0f},
        .ambient_color = {1.0f, 1.0f, 1.0f},
        .light_pos = {5.0f, 1.0f, 2.0f},
        .p_ecs_ctx = &ecs_ctx,
        .wireframe_mode = false,
    };

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
            render_camera_properties(nk_ctx, p_win, &cam, &cam_settings);

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

    scene_free(&scene);
    resources_free(resources);
    resources_unload_idstrings(&load_strings);

    nk_glfw3_shutdown(&nk_glfw);
    glfwTerminate();

    NFD_Quit();

    return 0;
}
