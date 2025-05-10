#include <editor.h>

#include <cxlist.h>

#include <model.h>
#include <primitive_resources.h>
#include <tinydir.h>

#include <hashmap.h>
#include <string.h>
#include <nfd.h>

#include <utils.h>
#include <camera.h>
#include <fl_ecs.h>
#include <resources.h>

static int fl_widget_ctx_cmp(const void *a, const void *b, void *udata) {
    (void) udata;
    const FlWidgetCtx *ra = a;
    const FlWidgetCtx *rb = b;

    return strcmp(ra->id, rb->id);
}

static uint64_t fl_widget_ctx_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const FlWidgetCtx *res = item;
    // default hash
    return hashmap_sip(res->id, strlen(res->id), seed0, seed1);
}

FlEditorCtx create_editor_ctx(void) {
    return (FlEditorCtx) {
        .widgets = hashmap_new(sizeof(FlWidgetCtx), 0, 0, 0, fl_widget_ctx_hash, fl_widget_ctx_cmp, NULL, NULL),
        .mode = FL_EDITOR_VIEW
    };
}

const char* editor_ctx_register_widget(FlEditorCtx ctx, FlWidgetCtx *p_widget_ctx) {
    assert(p_widget_ctx && "ERROR: cannot register a NULL widget -> p_widget_ctx is NULL");
    assert(p_widget_ctx->id && "ERROR: identifier is required to be initialized -> p_widget_ctx->id is NULL");
    hashmap_set(ctx.widgets, p_widget_ctx);
    return p_widget_ctx->id;
}

// TODO: make these const FlEditorCtx *p_ctx instead to be consistent with other piece of code
int editor_ctx_iter(FlEditorCtx ctx, size_t *p_iter, FlWidgetCtx **pp_widget_ctx) {
    return hashmap_iter(ctx.widgets, p_iter, (void**)pp_widget_ctx);
}

const FlWidgetCtx* editor_ctx_get_widget(const FlEditorCtx *ctx, const char *identifier) {
    return hashmap_get(ctx->widgets, &(FlWidgetCtx){ .id = identifier });
}

b8 editor_ctx_set_widget_open(FlEditorCtx ctx, const char *identifier, b8 is_open) {
    const FlWidgetCtx *p_widget = hashmap_get(ctx.widgets, &(FlWidgetCtx){ .id = identifier });

    if(!p_widget)
        return false;

    FlWidgetCtx new_ctx = *p_widget;
    new_ctx.is_open = is_open;

    hashmap_set(ctx.widgets, &new_ctx);
    return true;
}

b8 editor_ctx_is_widget_open(FlEditorCtx ctx, const char *identifier) {
    const FlWidgetCtx *scene_widget = hashmap_get(ctx.widgets, &(FlWidgetCtx){ .id = identifier });

    assert(scene_widget && "ERROR: scene widget is null, cannot check if the widget is open");
    
    return scene_widget->is_open;
}

void editor_ctx_free(FlEditorCtx ctx) {
    hashmap_free(ctx.widgets);
}





FlScaling __g_scaling_handle = {{1.0f, 1.0f}};

void scaling_set_dpi(FlScaling *p_handle, float xscale, float yscale) {
    p_handle->dpi_scale[0] = xscale;
    p_handle->dpi_scale[1] = yscale;
}

void g_scaling_set_dpi(float xscale, float yscale) {
    scaling_set_dpi(&__g_scaling_handle, xscale, yscale);
}


#define DPI_SCALEX(VALUE) ((VALUE) * __g_scaling_handle.dpi_scale[0])
#define DPI_SCALEY(VALUE) ((VALUE) * __g_scaling_handle.dpi_scale[1])

float g_scaling_x(float xscale) {
    return DPI_SCALEX(xscale);
}

float g_scaling_y(float yscale) {
    return DPI_SCALEY(yscale);
}




const nk_flags DEFAULT_NK_WIN_FLAGS = NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
    NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE;

const nk_flags EMPTY_NK_WIN_FLAGS = 0;

const float HUGE_VALUEF = 9999999999.0f;

void fl_xyz_widget(struct nk_context *p_ctx, const char *name, vec3 p_pos, float min, float max) {
    assert(min <= max && "the min value should always be smaller or equal to the max value");

    nk_layout_row_dynamic(p_ctx, DPI_SCALEY(18), 4);
    nk_label(p_ctx, name, NK_TEXT_LEFT);
    nk_property_float(p_ctx, "#x:", min, &p_pos[0], max, 0.1f, 0.2f);
    nk_property_float(p_ctx, "#y:", min, &p_pos[1], max, 0.1f, 0.2f);
    nk_property_float(p_ctx, "#z:", min, &p_pos[2], max, 0.1f, 0.2f);
}

void fl_combo_color_picker_vec4(struct nk_context *p_ctx, vec4 *p_color) {
    struct nk_colorf nk_obj_color = { (*p_color)[0], (*p_color)[1], (*p_color)[2], (*p_color)[3] };

    fl_combo_color_picker(p_ctx, &nk_obj_color);

    (*p_color)[0] = nk_obj_color.r;
    (*p_color)[1] = nk_obj_color.g;
    (*p_color)[2] = nk_obj_color.b;
    (*p_color)[3] = nk_obj_color.a;
}

void fl_combo_color_picker_vec3(struct nk_context *p_ctx, vec3 *p_color) {
    struct nk_colorf nk_obj_color = { (*p_color)[0], (*p_color)[1], (*p_color)[2], 1.0f };

    fl_combo_color_picker(p_ctx, &nk_obj_color);

    (*p_color)[0] = nk_obj_color.r;
    (*p_color)[1] = nk_obj_color.g;
    (*p_color)[2] = nk_obj_color.b;
}

void fl_combo_color_picker(struct nk_context *p_ctx, struct nk_colorf *p_color) {
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

#define NK_AUTO_LAYOUT 0

// hierarchy
void render_scene_hierarchy(struct nk_context *p_ctx, Scene *p_scene, FlEditorComponents *p_comps, Resources resources) {
    if (nk_begin(p_ctx, "Scene Hierarchy", nk_rect(DPI_SCALEX(20), DPI_SCALEY(50),
                                                    DPI_SCALEX(340), DPI_SCALEY(250)), DEFAULT_NK_WIN_FLAGS)) {
        const Model *p_model = resources_find(resources, "primitives/cube");

        nk_menubar_begin(p_ctx);

        nk_layout_row_begin(p_ctx, NK_STATIC, DPI_SCALEY(25), 3);

        FlEcsCtx *p_ecs_ctx = p_scene->p_ecs_ctx;

        const FlComponent transform_id = p_comps->transform;
        const FlComponent mesh_render_id = p_comps->mesh_render;
        const FlComponent g_light_id = p_comps->dir_light;
        const FlComponent meta_id = p_comps->meta;

        GLuint *p_add_icon = resources_find(resources, "textures/plus");


        nk_layout_row_push(p_ctx, DPI_SCALEX(25));
        if(nk_menu_begin_image(p_ctx, "Add", nk_image_id(*p_add_icon), nk_vec2(DPI_SCALEX(110),DPI_SCALEY(120)))) {
            nk_layout_row_dynamic(p_ctx, NK_AUTO_LAYOUT, 1);

            if(nk_menu_item_label(p_ctx, "Object", NK_TEXT_LEFT)) {
                const FlEntityId entity = fl_ecs_entity_add(p_ecs_ctx);

                fl_ecs_entity_activate_component(p_ecs_ctx, entity, transform_id, true);
                fl_ecs_entity_activate_component(p_ecs_ctx, entity, mesh_render_id, true);
                fl_ecs_entity_activate_component(p_ecs_ctx, entity, meta_id, true);

                FlTransform *p_transform = fl_ecs_get_entity_component_data(p_ecs_ctx, entity, transform_id);
                *p_transform = (FlTransform){.pos = GLM_VEC3_ZERO_INIT, .scale = GLM_VEC3_ONE_INIT};

                FlMeshRender *p_render = fl_ecs_get_entity_component_data(p_ecs_ctx, entity, mesh_render_id);

                Shader *p_phong = resources_find(resources, "shaders/phong");

                GLuint *p_diffuse_texture  = resources_find(resources, "textures/white1x1");
                assert(p_diffuse_texture);

                GLuint *p_specular_texture = resources_find(resources, "textures/white1x1");
                assert(p_specular_texture);

                *p_render = (FlMeshRender){
                    .p_model = p_model, .p_shader = p_phong,
                    .p_specular_tex = p_specular_texture, .p_diffuse_tex = p_diffuse_texture, .specular_factor = 32.0f
                };

                FlMeta *p_meta = fl_ecs_get_entity_component_data(p_ecs_ctx, entity, meta_id);
                strncpy(p_meta->name, "__UNNAMED__", FL_META_NAME_LEN_MAX);
            }

            // TODO: rename directional light as global light
            if(nk_menu_item_label(p_ctx, "Global Light", NK_TEXT_LEFT)) {
                const FlEntityId entity = fl_ecs_entity_add(p_ecs_ctx);
                fl_ecs_entity_activate_component(p_ecs_ctx, entity, g_light_id, true);

                FlGlobalLight *p_light = fl_ecs_get_entity_component_data(p_ecs_ctx, entity, g_light_id);
                *p_light = (FlGlobalLight) {
                    .dir = {0.0f, -1.0f, 0.0f},
                    .color = {1.0f, 1.0f, 1.0f}
                };

                fl_ecs_entity_activate_component(p_ecs_ctx, entity, meta_id, true);

                FlMeta *p_meta = fl_ecs_get_entity_component_data(p_ecs_ctx, entity, meta_id);
                strncpy(p_meta->name, "__UNNAMED_DIRLIGHT__", FL_META_NAME_LEN_MAX);
            }

            nk_menu_end(p_ctx);
        }

        nk_menubar_end(p_ctx);
        nk_layout_row_end(p_ctx);

        
        size_t iter = 0;
        FlEntityId entity;

        if (nk_tree_push(p_ctx, NK_TREE_TAB, "Entities", NK_MAXIMIZED)) {
            char txt_buf[FL_META_NAME_LEN_MAX + 256] = {0};

            FlEntityId entity_to_delete;
            bool should_delete_entity = false;

            while(fl_ecs_iter(p_ecs_ctx, &iter, &entity)) {
                nk_layout_row_template_begin(p_ctx, DPI_SCALEY(25));
                nk_layout_row_template_push_dynamic(p_ctx);
                nk_layout_row_template_push_static(p_ctx, DPI_SCALEX(25));
                nk_layout_row_template_end(p_ctx);

                if(fl_ecs_entity_has_component(p_ecs_ctx, entity, meta_id)) {
                    FlMeta *p_meta = fl_ecs_get_entity_component_data(p_ecs_ctx, entity, meta_id);
                    snprintf(txt_buf, arr_size(txt_buf), "[%zu] %s", entity, p_meta->name);
                }
                else
                    snprintf(txt_buf, arr_size(txt_buf), "[%zu] Entity", entity);

                nk_bool is_chosen = p_scene->selected_entity == entity;

                if(nk_selectable_label(p_ctx, txt_buf, NK_TEXT_LEFT, &is_chosen))
                    p_scene->selected_entity = entity;

                GLuint *p_del_icon = resources_find(resources, "textures/cross");

                if(nk_button_image(p_ctx, nk_image_id(*p_del_icon))) {
                    entity_to_delete = entity;
                    should_delete_entity = true;
                }
            }

            if(should_delete_entity)
                fl_ecs_entity_free(p_ecs_ctx, entity_to_delete);

            nk_tree_pop(p_ctx);
        }
    }
    nk_end(p_ctx);
}

void render_camera_properties(struct nk_context *p_ctx, GLFWwindow *p_win, Camera *p_cam, CameraSettings *p_cam_settings) {
    int width, height;
    glfwGetWindowSize(p_win, &width, &height);

    // camera properties
    if (nk_begin(p_ctx, "Camera Properties", nk_rect(width - DPI_SCALEX(340 + 20), DPI_SCALEY(50),
                                                      DPI_SCALEX(340), DPI_SCALEY(250)), DEFAULT_NK_WIN_FLAGS)) {
        nk_layout_row_dynamic(p_ctx, NK_AUTO_LAYOUT, 1);
        fl_xyz_widget(p_ctx, "Position: ", p_cam->pos, -HUGE_VALUEF, HUGE_VALUEF);

        nk_layout_row_dynamic(p_ctx, NK_AUTO_LAYOUT, 3);
        nk_label(p_ctx, "Rotation: ", NK_TEXT_LEFT);
        nk_property_float(p_ctx, "h(rad):", -FL_TAU, &p_cam->h_rot, FL_TAU, 0.1f, 0.01f);
        nk_property_float(p_ctx, "v(rad):", glm_rad(-89.0f), &p_cam->v_rot, glm_rad(89.0f), 0.1f, 0.01f);

        nk_layout_row_dynamic(p_ctx, NK_AUTO_LAYOUT, 1);
        nk_property_float(p_ctx, "Field of view(rad):", 0.01f, &p_cam->fov, FL_PI, 0.1f, 0.01f);
        nk_property_float(p_ctx, "near plane(meter):", 0.01f,  &p_cam->near, HUGE_VALUEF, 0.1f, 0.01f);
        nk_property_float(p_ctx, "far plane(meter):", 0.01f,   &p_cam->far,  HUGE_VALUEF, 0.1f, 0.01f);

        nk_property_float(p_ctx, "speed multiplier:", 0.01f, &p_cam_settings->speed_multiplier, HUGE_VALUEF, 0.1f, 0.01f);
    }
    nk_end(p_ctx);
}

void render_editor_metrics(struct nk_context *p_ctx, GLFWwindow *p_win, float dt) {
    int width, height;
    glfwGetWindowSize(p_win, &width, &height);

    struct nk_rect rect = nk_rect(width - DPI_SCALEX(250 + 20), DPI_SCALEY(50), DPI_SCALEX(250), DPI_SCALEY(50));


    if (nk_begin(p_ctx, "Editor Metrics", rect, EMPTY_NK_WIN_FLAGS)) {
        const struct nk_input *in = &p_ctx->input;

        float fps = 1.0f / dt;

        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(15), 1);
        nk_label(p_ctx, "Editor Metrics", NK_TEXT_CENTERED);
        nk_labelf(p_ctx, NK_TEXT_CENTERED, "frames per second(FPS): %.2fs", fps);

        if(nk_input_is_mouse_hovering_rect(in, rect))
            nk_tooltip(p_ctx, "Editor metrics are used to create items");
    }
    nk_end(p_ctx);
}

void resources_model_free(void *p_raw) {
    model_free(p_raw);
}

void render_resource_manager(struct nk_context *p_ctx, Resources resources) {
    if (nk_begin(p_ctx, "Resource Manager", nk_rect(DPI_SCALEX(20), DPI_SCALEY(400),
                                                    DPI_SCALEX(400), DPI_SCALEY(250)), DEFAULT_NK_WIN_FLAGS)) {
        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 2);
        
        GLuint *p_model_icon = resources_find(resources, "textures/cube");
        assert(p_model_icon && "ERROR: button image cannot be null -> p_model_icon is NULL");

        if(nk_button_image_label(p_ctx, nk_image_id(*p_model_icon), "load model", NK_TEXT_RIGHT)) {
            nfdu8char_t *model_path;
            nfdu8filteritem_t filters[] = { { "Wavefront Obj", "obj" } };

            nfdopendialogu8args_t args = {0};
            args.filterList = filters;
            args.filterCount = arr_size(filters);

            nfdresult_t result = NFD_OpenDialogU8_With(&model_path, &args);

            if (result == NFD_OKAY) {
                puts("Info: Found model at path\n");
                puts(model_path);

                tinydir_file file;

                ResIdStrings *p_id_strings = resources_lazy_get_id_strings(resources);

                if(tinydir_file_open(&file, model_path) == 0) {
                    printf("Loading model: %s\n", file.name);

                    Model *p_model = load_model_obj_tri(model_path);
                    char *dup_model_path = strdup(model_path);
                    resources_store(resources, &(Resource) {
                        .id = dup_model_path, .p_raw = p_model, .type = FL_RES_MODEL, .free = resources_model_free
                    });

                    list_append(p_id_strings, dup_model_path);

                    printf("Model loaded: %s\n", file.name);
                }

                NFD_FreePathU8(model_path);
            }
            else if (result == NFD_CANCEL) {
                puts("Info: pressed cancel.");
            }
            else {
                printf("Error: %s\n", NFD_GetError());
            }
        }
        
        size_t texs_idx = 0;
        Resource *res_texs[64] = {0};

        size_t models_idx = 0;
        Resource *res_models[64] = {0};

        size_t shaders_idx = 0;
        Resource *res_shaders[64] = {0};

        {
            size_t iter = 0;
            Resource *p_resource = NULL;

            while(resources_iter(resources, &iter, &p_resource)) {
                switch(p_resource->type) {
                    case FL_RES_TEXTURE:
                        res_texs[texs_idx++] = p_resource;
                        break;

                    case FL_RES_MODEL:
                        res_models[models_idx++] = p_resource;
                        break;

                    case FL_RES_SHADER_PROG:
                        res_shaders[shaders_idx++] = p_resource;
                        break;
                }
            }
        }


        // rendering
        if (nk_tree_push(p_ctx, NK_TREE_NODE, "Textures", NK_MINIMIZED)) {
            nk_layout_row_dynamic(p_ctx, DPI_SCALEY(20), 2);


            for(size_t i = 0; i < texs_idx - 1; i++) {
                Resource *p_res = res_texs[i];
                nk_labelf(p_ctx, NK_TEXT_LEFT, "[%zu] %s: ", i, p_res->id);
                nk_labelf(p_ctx, NK_TEXT_RIGHT, "%p", p_res->p_raw);
            }

            
            nk_tree_pop(p_ctx);
        }

        if (nk_tree_push(p_ctx, NK_TREE_NODE, "Models", NK_MINIMIZED)) {
            nk_layout_row_dynamic(p_ctx, DPI_SCALEY(20), 2);

            for(size_t i = 0; i < models_idx - 1; i++) {
                Resource *p_res = res_models[i];
                nk_labelf(p_ctx, NK_TEXT_LEFT, "[%zu] %s: ", i, p_res->id);
                nk_labelf(p_ctx, NK_TEXT_RIGHT, "%p", p_res->p_raw);
            }
            
            nk_tree_pop(p_ctx);
        }

        if (nk_tree_push(p_ctx, NK_TREE_NODE, "Shaders", NK_MINIMIZED)) {
            nk_layout_row_dynamic(p_ctx, DPI_SCALEY(20), 2);

            for(size_t i = 0; i < shaders_idx - 1; i++) {
                Resource *p_res = res_shaders[i];
                nk_labelf(p_ctx, NK_TEXT_LEFT, "[%zu] %s: ", i, p_res->id);
                nk_labelf(p_ctx, NK_TEXT_RIGHT, "%p", p_res->p_raw);
            }
            
            nk_tree_pop(p_ctx);
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

        nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 2);
        nk_label(p_ctx, "Clear Color", NK_TEXT_LEFT);
        fl_combo_color_picker_vec4(p_ctx, &p_scene->clear_color);

        nk_label(p_ctx, "Ambient Color", NK_TEXT_LEFT);
        fl_combo_color_picker_vec3(p_ctx, &p_scene->ambient_color);
    }
    nk_end(p_ctx);
}


void render_main_menubar(struct nk_context *p_ctx, GLFWwindow *p_win, Resources resources, FlEditorCtx *p_editor_ctx) {
    int width, height;
    glfwGetWindowSize(p_win, &width, &height);

    if (nk_begin(p_ctx, "Main Menubar", nk_rect(0, 0, width, DPI_SCALEY(30)), EMPTY_NK_WIN_FLAGS)) {
        GLuint *p_icon_tex = resources_find(resources, "textures/wrench");
        assert(p_icon_tex && "Error: Cannot load icon for main menu bar");

        struct nk_image widgets_icon = nk_image_id(*p_icon_tex);

        const size_t MENU_WIDGET_COUNT = 4;

        size_t common_size = 0;
        FlWidgetCtx *common_widgets[32] = {0};

        size_t settings_size = 0;
        FlWidgetCtx *settings_widgets[32] = {0};

        size_t metrics_size = 0;
        FlWidgetCtx *metrics_widgets[32] = {0};

        size_t iter = 0;
        FlWidgetCtx *p_widget_ctx = NULL;

        while(editor_ctx_iter(*p_editor_ctx, &iter, &p_widget_ctx)) {
            switch(p_widget_ctx->type) {
                case FL_WIDGET_COMMON:
                    common_widgets[common_size++] = p_widget_ctx;
                    break;

                case FL_WIDGET_SETTINGS:
                    settings_widgets[settings_size++] = p_widget_ctx;
                    break;

                case FL_WIDGET_METRICS:
                    metrics_widgets[metrics_size++] = p_widget_ctx;
                    break;
            }
        }

        nk_menubar_begin(p_ctx);

        nk_layout_row_begin(p_ctx, NK_STATIC, DPI_SCALEY(25), MENU_WIDGET_COUNT);
        nk_layout_row_push(p_ctx, DPI_SCALEY(95));

        const float MENU_LABEL_WIDTH = 160;

        // TODO: rewrite this to make this less longer, it is repeating too much
        if (nk_menu_begin_image_label(
            p_ctx, "common", NK_TEXT_RIGHT, widgets_icon,
            nk_vec2(DPI_SCALEX(MENU_LABEL_WIDTH), DPI_SCALEY(200))
        )) {

            for(size_t i = 0; i < common_size; i++) {
                FlWidgetCtx *p_widget_ctx = common_widgets[i];

                nk_bool is_open = p_widget_ctx->is_open;
                nk_layout_row_template_begin(p_ctx, DPI_SCALEY(20));

                if(p_widget_ctx->p_icon_tex)
                    nk_layout_row_template_push_static(p_ctx, DPI_SCALEY(20));

                nk_layout_row_template_push_dynamic(p_ctx);
                nk_layout_row_template_end(p_ctx);

                if(p_widget_ctx->p_icon_tex)
                    nk_image(p_ctx, nk_image_id(*p_widget_ctx->p_icon_tex));

                nk_checkbox_label(p_ctx, p_widget_ctx->id, &is_open);
                editor_ctx_set_widget_open(*p_editor_ctx, p_widget_ctx->id, is_open);
            }

            nk_menu_end(p_ctx);
        }

        nk_layout_row_push(p_ctx, DPI_SCALEY(95));

        if (nk_menu_begin_image_label(
            p_ctx, "settings", NK_TEXT_RIGHT, widgets_icon,
            nk_vec2(DPI_SCALEX(MENU_LABEL_WIDTH), DPI_SCALEY(200))
        )) {

            for(size_t i = 0; i < settings_size; i++) {
                FlWidgetCtx *p_widget_ctx = settings_widgets[i];

                nk_bool is_open = p_widget_ctx->is_open;
                nk_layout_row_template_begin(p_ctx, DPI_SCALEY(20));

                if(p_widget_ctx->p_icon_tex)
                    nk_layout_row_template_push_static(p_ctx, DPI_SCALEY(20));

                nk_layout_row_template_push_dynamic(p_ctx);
                nk_layout_row_template_end(p_ctx);

                if(p_widget_ctx->p_icon_tex)
                    nk_image(p_ctx, nk_image_id(*p_widget_ctx->p_icon_tex));

                nk_checkbox_label(p_ctx, p_widget_ctx->id, &is_open);
                editor_ctx_set_widget_open(*p_editor_ctx, p_widget_ctx->id, is_open);
            }

            nk_menu_end(p_ctx);
        }

        nk_layout_row_push(p_ctx, DPI_SCALEY(95));

        if (nk_menu_begin_image_label(
            p_ctx, "metrics", NK_TEXT_RIGHT, widgets_icon,
            nk_vec2(DPI_SCALEX(MENU_LABEL_WIDTH), DPI_SCALEY(200))
        )) {

            for(size_t i = 0; i < metrics_size; i++) {
                FlWidgetCtx *p_widget_ctx = metrics_widgets[i];

                nk_bool is_open = p_widget_ctx->is_open;
                nk_layout_row_template_begin(p_ctx, DPI_SCALEY(20));

                if(p_widget_ctx->p_icon_tex)
                    nk_layout_row_template_push_static(p_ctx, DPI_SCALEY(20));

                nk_layout_row_template_push_dynamic(p_ctx);
                nk_layout_row_template_end(p_ctx);

                if(p_widget_ctx->p_icon_tex)
                    nk_image(p_ctx, nk_image_id(*p_widget_ctx->p_icon_tex));

                nk_checkbox_label(p_ctx, p_widget_ctx->id, &is_open);
                editor_ctx_set_widget_open(*p_editor_ctx, p_widget_ctx->id, is_open);
            }

            nk_menu_end(p_ctx);
        }

        nk_layout_row_push(p_ctx, DPI_SCALEY(95));

        const FlWidgetCtx *p_tut_widget_ctx = editor_ctx_get_widget(p_editor_ctx, "tutorial");
        assert(p_tut_widget_ctx->p_icon_tex);

        nk_bool is_open = p_tut_widget_ctx->is_open;

        nk_checkbox_label(p_ctx, p_tut_widget_ctx->id, &is_open);
        editor_ctx_set_widget_open(*p_editor_ctx, p_tut_widget_ctx->id, is_open);

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

typedef b8(*res_filter_combo_func)(Resource *p_res);

b8 res_filter_combo_models(Resource *p_res) {
    return p_res->type == FL_RES_MODEL;
}

b8 res_filter_combo_textures(Resource *p_res) {
    return p_res->type == FL_RES_TEXTURE;
}

void* resources_filtered_combo_selection(
    Resources res, struct nk_context *p_ctx,
    const void *current_resource, const char **list, size_t list_max_size, res_filter_combo_func filter_func) {
    assert(filter_func && "ERR: filter function cannot be NULL");
    size_t r_iter = 0;
    Resource *p_resource = NULL;

    size_t list_size = 0;

    int selected = 0;

    while(resources_iter(res, &r_iter, &p_resource) && list_size < list_max_size) {
        if(filter_func(p_resource)) {
            if(p_resource->p_raw == current_resource)
                selected = list_size;

            list[list_size++] = p_resource->id;
        }
    }

    selected = nk_combo(p_ctx, list, list_size, selected, DPI_SCALEY(15), nk_vec2(DPI_SCALEX(300), DPI_SCALEY(200)));

    return resources_find(res, list[selected]);
}

void render_entity_inspector(struct nk_context *p_ctx, Scene *p_scene, Resources resources,
                             FlEditorComponents *p_comps) {
    if (nk_begin(p_ctx, "Entity Inspector",
                 nk_rect(DPI_SCALEX(20), DPI_SCALEY(500), DPI_SCALEX(300), DPI_SCALEY(400)),
                 DEFAULT_NK_WIN_FLAGS)) {
        FlEcsCtx *p_ecs_ctx = p_scene->p_ecs_ctx;


        if(fl_ecs_entity_valid(p_ecs_ctx, p_scene->selected_entity)) {
            const FlComponent transform_id = p_comps->transform;

            if(fl_ecs_entity_has_component(p_ecs_ctx, p_scene->selected_entity, p_comps->meta)) {
                FlMeta *p_meta = fl_ecs_get_entity_component_data(p_ecs_ctx, p_scene->selected_entity, p_comps->meta);
                nk_layout_row_dynamic(p_ctx, NK_AUTO_LAYOUT, 1);
                nk_labelf(p_ctx, NK_TEXT_CENTERED, "%s", p_meta->name);
            }

            // TODO: this should be separated, each component should handle their own rendering, instead of being rendered here
            // RENDER TRANSFORM
            if(fl_ecs_entity_has_component(p_ecs_ctx, p_scene->selected_entity, transform_id)) {
                nk_layout_row_dynamic(p_ctx, NK_AUTO_LAYOUT, 1);
                if(nk_tree_push(p_ctx, NK_TREE_TAB, "Transform", NK_MAXIMIZED)) {
                    FlTransform *p_transform = fl_ecs_get_entity_component_data(p_ecs_ctx, p_scene->selected_entity, transform_id);

                    fl_xyz_widget(p_ctx, "Position", p_transform->pos, -HUGE_VALUEF, HUGE_VALUEF);
                    fl_xyz_widget(p_ctx, "Rotation", p_transform->rot, -FL_TAU, FL_TAU);
                    fl_xyz_widget(p_ctx, "Scale", p_transform->scale, -HUGE_VALUEF, HUGE_VALUEF);
                    nk_tree_pop(p_ctx);
                }
            }

            // RENDER MESH RENDER
            const FlComponent mesh_render_id = p_comps->mesh_render;
            if(fl_ecs_entity_has_component(p_ecs_ctx, p_scene->selected_entity, mesh_render_id)) {
                nk_layout_row_dynamic(p_ctx, NK_AUTO_LAYOUT, 1);

                if(nk_tree_push(p_ctx, NK_TREE_TAB, "Mesh Render", NK_MAXIMIZED)) {
                    FlMeshRender *p_mesh_render = fl_ecs_get_entity_component_data(p_ecs_ctx, p_scene->selected_entity, mesh_render_id);

                    const char *models[64] = {0};

                    nk_layout_row_dynamic(p_ctx, NK_AUTO_LAYOUT, 2);
                    nk_label(p_ctx, "Model:", NK_TEXT_LEFT);
                    Model *p_chose_model = resources_filtered_combo_selection(
                        resources, p_ctx, p_mesh_render->p_model, models, arr_size(models), res_filter_combo_models
                    );

                    p_mesh_render->p_model = p_chose_model;


                    // TODO: requires fall back texture
                    const char *textures[64] = {0};

                    nk_layout_row_dynamic(p_ctx, NK_AUTO_LAYOUT, 2);
                    nk_label(p_ctx, "Diffuse Map:", NK_TEXT_LEFT);
                    GLuint *p_diffuse_tex = resources_filtered_combo_selection(
                        resources, p_ctx, p_mesh_render->p_diffuse_tex, textures, arr_size(textures), res_filter_combo_textures
                    );

                    p_mesh_render->p_diffuse_tex = p_diffuse_tex;

                    assert(p_mesh_render->p_diffuse_tex);
                    struct nk_image diffuse_map = nk_image_id(*p_mesh_render->p_diffuse_tex);
                    nk_layout_row_dynamic(p_ctx, NK_AUTO_LAYOUT, 1);

                    nk_layout_row_begin(p_ctx, NK_STATIC, DPI_SCALEY(50), 1);
                    nk_layout_row_push(p_ctx, DPI_SCALEX(50));
                    nk_image(p_ctx, diffuse_map);
                    nk_layout_row_end(p_ctx);

                    nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 2);
                    nk_label(p_ctx, "Specular Map:", NK_TEXT_LEFT);
                    GLuint *p_specular_tex = resources_filtered_combo_selection(
                        resources, p_ctx, p_mesh_render->p_specular_tex, textures, arr_size(textures), res_filter_combo_textures
                    );

                    p_mesh_render->p_specular_tex = p_specular_tex;

                    assert(p_mesh_render->p_specular_tex);
                    struct nk_image specular_map = nk_image_id(*p_mesh_render->p_specular_tex);

                    nk_layout_row_begin(p_ctx, NK_STATIC, DPI_SCALEY(50), 1);
                    nk_layout_row_push(p_ctx, DPI_SCALEX(50));
                    nk_image(p_ctx, specular_map);
                    nk_layout_row_end(p_ctx);

                    nk_layout_row_dynamic(p_ctx, DPI_SCALEY(25), 1);
                    nk_property_float(p_ctx, "Specular Factor", 0, &p_mesh_render->specular_factor, 500, 0.1f, 0.01f);

                    nk_tree_pop(p_ctx);
                }
            }

            const FlComponent dir_light_id = p_comps->dir_light;
            if(fl_ecs_entity_has_component(p_ecs_ctx, p_scene->selected_entity, dir_light_id)) {
                FlGlobalLight *p_dir_light = fl_ecs_get_entity_component_data(p_ecs_ctx, p_scene->selected_entity, dir_light_id);

                nk_layout_row_dynamic(p_ctx, DPI_SCALEX(25), 1);
                if(nk_tree_push(p_ctx, NK_TREE_TAB, "Directional Light", NK_MAXIMIZED)) {
                    fl_xyz_widget(p_ctx, "Direction", p_dir_light->dir, -1, 1);

                    nk_layout_row_dynamic(p_ctx, DPI_SCALEX(25), 2);
                    nk_label(p_ctx, "Color", NK_TEXT_LEFT);
                    fl_combo_color_picker_vec3(p_ctx, &p_dir_light->color);

                    nk_tree_pop(p_ctx);
                }
            }
        }
        else {
            nk_layout_row_dynamic(p_ctx, NK_AUTO_LAYOUT, 1);
            nk_label(p_ctx, "Select an Entity in the Scene Hierarchy", NK_TEXT_CENTERED);
            nk_label(p_ctx, "to view their components!", NK_TEXT_CENTERED);
        }
    }
    nk_end(p_ctx);
}

void render_tutorial(struct nk_context *p_ctx, vec2 win_size, Resources resources) {
    (void) resources;
    struct nk_rect rect = nk_rect(0, 0, DPI_SCALEX(700), DPI_SCALEY(500));
    rect.x = (win_size[0] - rect.w) * 0.5f;
    rect.y = (win_size[1] - rect.h) * 0.5f;
    
    if (nk_begin(p_ctx, "Tutorial", rect, DEFAULT_NK_WIN_FLAGS)) {
        nk_layout_row_dynamic(p_ctx, NK_AUTO_LAYOUT, 1);
        nk_label_wrap(p_ctx, "This tutorial serves as a basic guide on how to navigate, use and create stuff in Flatova!");

        if(nk_tree_push(p_ctx, NK_TREE_NODE, "Interface", NK_MINIMIZED)) {
            nk_layout_row_dynamic(p_ctx, DPI_SCALEY(20), 1);
            nk_label_wrap(p_ctx, "The major interfaces of Flatova consists of the following:");

            nk_layout_row_dynamic(p_ctx, NK_AUTO_LAYOUT, 1);
            if(nk_tree_push(p_ctx, NK_TREE_TAB, "Menubar", NK_MINIMIZED)) {
                nk_layout_row_dynamic(p_ctx, DPI_SCALEY(20 * 2), 1);
                nk_label_wrap(p_ctx, "Menubar is the long strip of UI at the top of the application. " \
                              "It is the most important place to enable most of the functionalities in the editor.");
                nk_tree_pop(p_ctx);
            }

            if(nk_tree_push(p_ctx, NK_TREE_TAB, "Widgets", NK_MINIMIZED)) {
                nk_layout_row_dynamic(p_ctx, DPI_SCALEY(20 * 3), 1);
                nk_label_wrap(p_ctx, "Widgets in Flatova are considered to be tiny floating widgets. " \
                              "The tutorial window you see right now is a widget as well. They can be enabled by going to the menu bar");
                nk_tree_pop(p_ctx);
            }

            if(nk_tree_push(p_ctx, NK_TREE_TAB, "Viewport", NK_MINIMIZED)) {
                nk_layout_row_dynamic(p_ctx, DPI_SCALEY(20 * 3), 1);
                nk_label_wrap(p_ctx, "The viewport is the main renderer of the scene. " \
                              "what you see in the background is considered a Viewport!");

                nk_label_wrap(p_ctx, "In the viewport, you can hold right click to drag and look around. " \
                              "W A S D to move around the scene");
                nk_tree_pop(p_ctx);
            }

            if(nk_tree_push(p_ctx, NK_TREE_TAB, "Editor Metrics", NK_MINIMIZED)) {
                nk_layout_row_dynamic(p_ctx, DPI_SCALEY(20 * 3), 1);
                nk_label_wrap(p_ctx, "Editor metrics show up at the top right of the Viewport. " \
                              "They represent your Editor's current performance and some debug information.");
                nk_tree_pop(p_ctx);
            }

            nk_tree_pop(p_ctx);
        }

    }
    nk_end(p_ctx);
}
