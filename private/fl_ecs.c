#include <assert.h>
#include <fl_ecs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FlTable fl_ecs_table_create(FlEntity entity_cap, FlComponent component_cap) {
    FlTable table = (FlTable)malloc(sizeof(FlTableItem) * entity_cap * component_cap);
    memset(table, 0, entity_cap * component_cap);

    return table;
}

static void fl_ecs_table_free(FlTable table) {
    free(table);
}

static void** fl_ecs_data_lists_create(size_t component_cap) {
    void **data_lists = malloc(sizeof(void*) * component_cap);
    memset(data_lists, 0, sizeof(void*) * component_cap);

    return data_lists;
}

static void fl_ecs_data_lists_free(void **data_lists, size_t component_count) {
    for(size_t i = 0; i < component_count; i++)
        free(data_lists[i]);

    free(data_lists);
}

FlEcsCtx fl_ecs_ctx_create(FlEntity entity_cap, FlComponent component_cap) {
    return (FlEcsCtx) {
        .table = fl_ecs_table_create(entity_cap, component_cap),
        .data_lists = fl_ecs_data_lists_create(component_cap),
        .component_cap = component_cap, .entity_cap = entity_cap,

        .entity_count = 0,
        .g_entity_id = 0,
        .g_component_id = 0,
        .component_byte_sizes = malloc(sizeof(size_t) * component_cap)
    };
}

void fl_ecs_ctx_free(FlEcsCtx *p_ctx) {
    fl_ecs_data_lists_free(p_ctx->data_lists, p_ctx->component_cap);
    fl_ecs_table_free(p_ctx->table);
    free(p_ctx->component_byte_sizes);
}

static inline size_t fl_ecs_get_table_idx(FlEcsCtx *p_ctx, FlEntity entity, FlComponent component) {
    return component * p_ctx->entity_cap + entity;
}


FlComponent fl_ecs_add_component(FlEcsCtx *p_ctx, size_t component_bytes_size) {
    const FlComponent COMP_ID = p_ctx->g_component_id++;

    p_ctx->data_lists[COMP_ID]           = malloc(component_bytes_size * p_ctx->entity_cap);
    p_ctx->component_byte_sizes[COMP_ID] = component_bytes_size;

    return COMP_ID;
}

void fl_ecs_entity_activate_component(FlEcsCtx *p_ctx, FlEntity entity, FlComponent component, b8 active) {
    p_ctx->table[fl_ecs_get_table_idx(p_ctx, entity, component)] = active;
}

b8 fl_ecs_query(FlEcsCtx *p_ctx, size_t *p_iter, FlEntity *p_entity, FlComponent *components, size_t components_size) {
    // checking each and individual entity

    while(*p_iter < p_ctx->entity_cap) {
        b8 has_all_components = true;

        // check if the current component has all the required components
        for(size_t i_comp = 0; i_comp < components_size; i_comp++) {
            if(!p_ctx->table[fl_ecs_get_table_idx(p_ctx, *p_iter, components[i_comp])]) {
                has_all_components = false;
                break;
            }
        }

        if(has_all_components) {
            *p_entity = *p_iter; // TODO: coincidence? should we simplify this?

            *p_iter += 1;
            return true;
        }

        *p_iter += 1;
    }

    return false;
}

FlEntity fl_ecs_entity_add(FlEcsCtx *p_ctx) {
    assert(p_ctx->g_entity_id < p_ctx->entity_cap);

    p_ctx->entity_count += 1;
    return p_ctx->g_entity_id++;
}

void* fl_ecs_get_entity_component_data(FlEcsCtx *p_ctx, FlEntity entity, FlComponent component) {
    b8 *data_list = (b8*)p_ctx->data_lists[component];

    assert(data_list);

    const size_t component_byte_size = p_ctx->component_byte_sizes[component];
    return &data_list[component_byte_size * entity];
}

b8 fl_ecs_entity_has_component(FlEcsCtx *p_ctx, FlEntity entity, FlComponent component) {
    return p_ctx->table[fl_ecs_get_table_idx(p_ctx, entity, component)];
}


