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

#include <stb_image.h>

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

typedef struct DefaultPipeline_t {
    GLuint vert_buf;
    GLuint norm_buf;
    GLuint tex_coord_buf;

    GLuint vert_arr;
} DefaultPipeline;

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

        cam.h_rot += offset_x  * cam_settings.sensitivity;
        cam.v_rot += -offset_y * cam_settings.sensitivity;

        cam.v_rot = clampf(cam.v_rot, -cam_settings.max_vrot, cam_settings.max_vrot);

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

void render_grid(Resources resources, const DefaultPipeline pipeline, Camera *p_cam, mat4 view_proj_mat) {
    Shader *p_grid = (Shader*)resources_find(resources, "shaders/grid");

    shader_use(*p_grid); {
        vertex_load_buffers(
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


void render_skybox(Resources resources, const DefaultPipeline pipeline, Camera *p_cam, mat4 view_proj_mat) {
    Shader *p_sky_shader = resources_find(resources, "shaders/skybox"   );
    Model  *p_sky_model  = resources_find(resources, "primitives/skybox");
    GLuint *p_sky_tex    = resources_find(resources, "textures/skymap_1");

    shader_use(*p_sky_shader); {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *p_sky_tex);

        vertex_load_buffers(
            p_sky_model->verts, p_sky_model->verts_count,
            pipeline.vert_buf, pipeline.norm_buf, pipeline.tex_coord_buf
        );

        shader_set_uniform_1i(*p_sky_shader, "skymap", 0);

        shader_set_uniform_mat4fv(*p_sky_shader, "view_proj", view_proj_mat);

        mat4 local_to_world_mat;
        glm_mat4_identity(local_to_world_mat);
        // TODO: we should not do this, the perspective calculation should be handled separately
        // where near plane should not affect the skybox
        glm_translate(local_to_world_mat, p_cam->pos);
        glm_scale(local_to_world_mat, (vec3){p_cam->far, p_cam->far, p_cam->far});

        shader_set_uniform_mat4fv(*p_sky_shader, "model", local_to_world_mat);

        glDrawArrays(GL_TRIANGLES, 0, p_sky_model->verts_count);
    }
}

void render_scene_frame(GLFWwindow *p_win, const DefaultPipeline pipeline, Scene *p_scene,
                        Resources resources, FlEditorComponents comps) {
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
    glEnable(GL_STENCIL_TEST);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    FlEcsCtx *p_ecs_ctx = p_scene->p_ecs_ctx;

    size_t iter = 0;
    FlEntityId entity;

    FlComponent transform_comp = comps.transform;
    FlComponent mesh_render_comp = comps.mesh_render;
    FlComponent dir_light_comp = comps.dir_light;

    vec3 global_light_dir = {0.0, 0.0, 0.0};
    vec3 global_light_color = {0.0, 0.0, 0.0};

    while(fl_ecs_query(p_ecs_ctx, &iter, &entity, &dir_light_comp, 1)) {
        FlDirLight *p_dir_light = fl_ecs_get_entity_component_data(p_ecs_ctx, entity, dir_light_comp);

        glm_vec3_copy(p_dir_light->direction, global_light_dir);
        glm_vec3_normalize(global_light_dir);

        glm_vec3_copy(p_dir_light->color, global_light_color);
        break;
    }

    iter = 0;
    FlComponent query_components[] = {
        transform_comp, mesh_render_comp
    };

    while(fl_ecs_query(p_ecs_ctx, &iter, &entity, query_components, arr_size(query_components))) {
        FlMeshRender *p_mesh_render = fl_ecs_get_entity_component_data(p_ecs_ctx, entity, mesh_render_comp);
        FlTransform *p_transform    = fl_ecs_get_entity_component_data(p_ecs_ctx, entity, transform_comp);
        assert(p_mesh_render);
        assert(p_transform);

        const Shader *p_shader = p_mesh_render->p_shader;
        const Model *p_model   = p_mesh_render->p_model;

        GLuint *p_diffuse_texture  = p_mesh_render->p_diffuse_tex;
        if(p_diffuse_texture == NULL) {
            p_diffuse_texture = resources_find(resources, "textures/white1x1");
        }

        GLuint *p_specular_texture = p_mesh_render->p_specular_tex;
        if(p_specular_texture == NULL) {
            p_specular_texture = resources_find(resources, "textures/white1x1");
        }

        assert(p_diffuse_texture);
        assert(p_specular_texture);

        vertex_load_buffers(
            p_model->verts,
            p_model->verts_count,
            pipeline.vert_buf, pipeline.norm_buf, pipeline.tex_coord_buf
        );

        mat4 local_to_world_mat = GLM_MAT4_IDENTITY_INIT;
        transform_apply(*p_transform, local_to_world_mat);


        // the normal deserves its own matrix, due to world material possibly scaling the normal vectors!
        mat4 normal_mat = GLM_MAT4_IDENTITY_INIT;
        euler_radians_transform_xyz(p_transform->rot, normal_mat);

        shader_use(*p_shader); {
            shader_set_uniform_3f(*p_shader, "dir_light.dir",   global_light_dir);
            shader_set_uniform_3f(*p_shader, "dir_light.color", global_light_color);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, *p_diffuse_texture);

            shader_set_uniform_1i(*p_shader, "material.diffuse", 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, *p_specular_texture);

            shader_set_uniform_1i(*p_shader, "material.specular", 1);

            shader_set_uniform_1f(*p_shader, "material.specular_factor", p_mesh_render->specular_factor);

            shader_set_uniform_3f(*p_shader, "material.ambient", p_scene->ambient_color);

            shader_set_uniform_3f(*p_shader, "cam_pos", cam.pos);
            shader_set_uniform_mat4fv(*p_shader, "view_proj", view_proj_mat);

            shader_set_uniform_mat4fv(*p_shader, "model", local_to_world_mat);

            shader_set_uniform_mat4fv(*p_shader, "norm_mat", normal_mat);
        }

        if(p_scene->wireframe_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // should render outline
        if(entity == p_scene->selected_entity) {
            glClear(GL_STENCIL_BUFFER_BIT);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // only set stencil to 1 if they show up on screen
            glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments pass
            glStencilMask(0xFF); // allow writing to stencil
        }

        glBindVertexArray(pipeline.vert_arr);
        glDrawArrays(GL_TRIANGLES, 0, p_model->verts_count);

        // draw outline
        if(entity == p_scene->selected_entity) {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00); // disable writing
            // glDisable(GL_DEPTH_TEST);

            Shader *p_single_color = resources_find(resources, "shaders/single_color");
            shader_use(*p_single_color); {
                mat4 outline_scale = GLM_MAT4_IDENTITY_INIT;
                const float SCALE_SIZE = 1.05f;
                glm_scale(outline_scale, (vec3){ SCALE_SIZE, SCALE_SIZE, SCALE_SIZE });

                mat4 scaled_local_to_world_mat;
                glm_mat4_mul(local_to_world_mat, outline_scale, scaled_local_to_world_mat);
                shader_set_uniform_mat4fv(*p_single_color, "model", scaled_local_to_world_mat);

                shader_set_uniform_mat4fv(*p_single_color, "view_proj", view_proj_mat);
                shader_set_uniform_3f(*p_single_color, "color", (vec3){0.0f, 1.0f, 0.0f});
            }

            glBindVertexArray(pipeline.vert_arr);
            glDrawArrays(GL_TRIANGLES, 0, p_model->verts_count);

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            // glEnable(GL_DEPTH_TEST);
        }

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

    const GLFWvidmode *primary = glfwGetVideoMode(glfwGetPrimaryMonitor());

    GLFWwindow *p_win = glfwCreateWindow(
        primary->width, primary->height, "Flatova", NULL, NULL
    );

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

typedef struct FlId2Idx_t {
    FlEntityId id;
    size_t idx;
} FlId2Idx;

int main(void) {
    const size_t TBL_ENTITY_COUNT    = 100;
    const size_t TBL_COMPONENT_COUNT = 32;

    FlEcsCtx ecs_ctx = fl_create_ecs_ctx(TBL_ENTITY_COUNT, TBL_COMPONENT_COUNT);

    const FlComponent TRANSFORM_ID = fl_ecs_add_component(
        &ecs_ctx, sizeof(FlTransform)
    );

    const FlComponent MESH_RENDER_ID = fl_ecs_add_component(
        &ecs_ctx, sizeof(FlMeshRender)
    );

    const FlComponent DIR_LIGHT_ID = fl_ecs_add_component(
        &ecs_ctx, sizeof(FlDirLight)
    );

    const FlComponent META_ID = fl_ecs_add_component(
        &ecs_ctx, sizeof(FlMeta)
    );

    FlEditorComponents comps = {
        .transform   = TRANSFORM_ID,
        .mesh_render = MESH_RENDER_ID,
        .dir_light   = DIR_LIGHT_ID,
        .meta        = META_ID
    };

    NFD_Init();

    Resources resources = resources_create();

    char cwd_path[512];
    getcwd(cwd_path, arr_size(cwd_path));
    printf("running on working directory: %s\n", cwd_path);

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

        resources_store(
            resources,
            &(Resource){ .id = "fonts/roboto_regular", .p_raw = roboto_regular, .type = FL_RES_FONT }
        );
    }
    
    // loop through contents in vendor
    const size_t DIR_DEPTH = 5;
    resources_load_dir_recursive(resources, DIR_DEPTH, "./vendor");

    GLFWimage img = {0};
    
    int nchannels;
    img.pixels = stbi_load("fl_logo_iter1.png", &img.width, &img.height, &nchannels, 0);

    glfwSetWindowIcon(p_win, 1, &img);

    stbi_image_free(img.pixels);


    // TODO: LOAD all shaders in vendor using recursive loading instead,
    // this method is tedious and makes the code too long
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

    if(resources_load_shader_from_files(
        resources, "shaders/depth_only",
        "./shaders/depth_only.vs", "./shaders/depth_only.fs"
    ) == NULL) {
        printf("Error: Failed to load depth shaders!\n");
        glfwTerminate();
        return -1;
    }

    printf("depth Shaders loaded\n");
    
    if(resources_load_shader_from_files(
        resources, "shaders/single_color",
        "./shaders/single_color.vs", "./shaders/single_color.fs"
    ) == NULL) {
        printf("Error: Failed to load single color shaders!\n");
        glfwTerminate();
        return -1;
    }

    printf("single Color Shaders loaded\n");

    if(resources_load_shader_from_files(
        resources, "shaders/outline",
        "./shaders/outline.vs", "./shaders/outline.fs"
    ) == NULL) {
        printf("Error: Failed to load outline shaders!\n");
        glfwTerminate();
        return -1;
    }

    printf("outline Shaders loaded\n");

    resources_store(
        resources,
        &(Resource){.id = "primitives/plane", .p_raw = get_prim_plane_vertices(), .type = FL_RES_VERTICES}
    );

    // create buffers
    VertexPipeline vert_pipeline = vertex_gen_buffer_arrays();

    DefaultPipeline pipe = {
        .vert_buf      = vert_pipeline.vert_buf,
        .vert_arr      = vert_pipeline.vert_arr,
        .tex_coord_buf = vert_pipeline.tex_coord_buf,
        .norm_buf      = vert_pipeline.normal_buf
    };

    glfwSetCursorPosCallback(p_win, glfw_cursor_callback);

    float dt = glfwGetTime();
    float prev_time = glfwGetTime();

    Scene scene = {
        .clear_color = {0.0f, 0.0f, 0.0f, 1.0f},
        .ambient_color = {0.88f, 0.69f, 0.61f},
        .light_pos = {5.0f, 1.0f, 2.0f},
        .p_ecs_ctx = &ecs_ctx,
        .selected_entity = 0,
        .wireframe_mode = false,
    };
    
    FlEditorCtx editor_ctx = create_editor_ctx();

    editor_ctx_register_widget(
        editor_ctx,
        &(FlWidgetCtx){ .id = "scene hierarchy", .type = FL_WIDGET_COMMON, .p_icon_tex = resources_find(resources, "textures/cube")}
    );
    editor_ctx_register_widget(
        editor_ctx,
        &(FlWidgetCtx){ .id = "camera properties", .type = FL_WIDGET_SETTINGS, .p_icon_tex = resources_find(resources, "textures/camera")}
    );
    editor_ctx_register_widget(
        editor_ctx,
        &(FlWidgetCtx){ .id = "resource viewer", .type = FL_WIDGET_COMMON, .p_icon_tex = resources_find(resources, "textures/magnify")}
    );
    editor_ctx_register_widget(
        editor_ctx,
        &(FlWidgetCtx){ .id = "scene settings", .type = FL_WIDGET_SETTINGS, .p_icon_tex = resources_find(resources, "textures/cog")}
    );
    editor_ctx_register_widget(
        editor_ctx,
        &(FlWidgetCtx){ .id = "file browser", .type = FL_WIDGET_COMMON, .p_icon_tex = resources_find(resources, "textures/folder")}
    );
    editor_ctx_register_widget(
        editor_ctx,
        &(FlWidgetCtx){ .id = "entity inspector", .type = FL_WIDGET_COMMON,  .p_icon_tex = resources_find(resources, "textures/eye")}
    );
    editor_ctx_register_widget(
        editor_ctx,
        &(FlWidgetCtx){ .id = "console", .type = FL_WIDGET_COMMON, .p_icon_tex = resources_find(resources, "textures/question")}
    );

    while(!glfwWindowShouldClose(p_win)) {
        float current_time = glfwGetTime();
        dt = current_time - prev_time;
        prev_time = current_time;

        process_input(p_win, dt);

        nk_glfw3_new_frame(&nk_glfw);

        render_main_menubar(nk_ctx, p_win, resources, &editor_ctx);

        static vec3 original_location;
        static double orig_grab_x, orig_grab_y;

        if(glfwGetKey(p_win, GLFW_KEY_G) == GLFW_PRESS && editor_ctx.mode != FL_EDITOR_GRAB) {

            if(fl_ecs_entity_valid(&ecs_ctx, scene.selected_entity) &&
                fl_ecs_entity_has_component(&ecs_ctx, scene.selected_entity, TRANSFORM_ID)) {
                FlTransform *p_transform = fl_ecs_get_entity_component_data(&ecs_ctx, scene.selected_entity, TRANSFORM_ID);

                glfwGetCursorPos(p_win, &orig_grab_x, &orig_grab_y);
                
                editor_ctx.mode = FL_EDITOR_GRAB;
                glm_vec3_copy(p_transform->pos, original_location);

                printf("Editor Mode Enter: Grab\n");
                fflush(stdout);
            }

        }

        if(editor_ctx.mode == FL_EDITOR_GRAB) {
            const float GRAB_UNIT_MULTIPLIER = 0.001f;
            vec3 right, up;
            camera_right(cam, right);
            camera_up(cam, up);

            static double new_grab_x, new_grab_y;
            glfwGetCursorPos(p_win, &new_grab_x, &new_grab_y);

            vec2 grab_diff = {
                new_grab_x - orig_grab_x,
                new_grab_y - orig_grab_y
            };

            vec3 offset;
            glm_vec3_zero(offset);
            glm_vec3_scale(right, grab_diff[0] * GRAB_UNIT_MULTIPLIER, right);
            glm_vec3_scale(up,    -grab_diff[1] * GRAB_UNIT_MULTIPLIER, up);

            glm_vec3_add(offset, right, offset);
            glm_vec3_add(offset, up, offset);


            if(fl_ecs_entity_valid(&ecs_ctx, scene.selected_entity) &&
                fl_ecs_entity_has_component(&ecs_ctx, scene.selected_entity, TRANSFORM_ID)) {
                FlTransform *p_transform = fl_ecs_get_entity_component_data(&ecs_ctx, scene.selected_entity, TRANSFORM_ID);
            
                glm_vec3_copy(original_location, p_transform->pos);
                glm_vec3_add(offset, p_transform->pos, p_transform->pos);

            }

            if(glfwGetMouseButton(p_win, GLFW_MOUSE_BUTTON_LEFT)) {
                editor_ctx.mode = FL_EDITOR_VIEW;

                printf("Applying transform\n");
            }
        }


        // TODO: remove the selected entity since it is by default passed into the scene itself
        if(editor_ctx_is_widget_open(editor_ctx, "scene hierarchy"))
            render_scene_hierarchy(nk_ctx, &scene, &comps, resources);

        if(editor_ctx_is_widget_open(editor_ctx, "camera properties"))
            render_camera_properties(nk_ctx, p_win, &cam, &cam_settings);

        if(editor_ctx_is_widget_open(editor_ctx, "resource viewer"))
            render_resource_viewer(nk_ctx, resources);

        if(editor_ctx_is_widget_open(editor_ctx, "scene settings"))
            render_scene_settings(nk_ctx, &scene);

        if(editor_ctx_is_widget_open(editor_ctx, "file browser"))
            render_file_browser(nk_ctx);

        if(editor_ctx_is_widget_open(editor_ctx, "entity inspector"))
            render_entity_inspector(nk_ctx, &scene, resources, &comps);

        render_editor_metrics(nk_ctx, p_win, dt);

        // RENDERING
        render_scene_frame(p_win, pipe, &scene, resources, comps);

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

    nk_glfw3_shutdown(&nk_glfw);
    glfwTerminate();

    NFD_Quit();

    return 0;
}
