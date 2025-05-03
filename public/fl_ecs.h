#pragma once
#ifndef _FL_ECS_H
#define _FL_ECS_H

#include <utils.h>

typedef size_t FlEntity;
typedef size_t FlComponent;

typedef b8 FlTableItem;
typedef FlTableItem* FlTable;

typedef struct FlEcsCtx_t {
    FlTable table;
    void **data_lists;

    size_t entity_count;

    size_t g_entity_id;
    size_t entity_cap;

    size_t g_component_id;
    size_t component_cap;

    size_t *component_byte_sizes;
} FlEcsCtx;

FlEcsCtx fl_create_ecs_ctx(size_t entity_cap, size_t component_cap);

void fl_ecs_ctx_free(FlEcsCtx *p_ctx);

FlComponent fl_ecs_add_component(FlEcsCtx *p_ctx, size_t component_bytes_size);

void fl_ecs_entity_activate_component(FlEcsCtx *p_ctx, FlEntity entity, FlComponent component, b8 active);

// Iterates through all the entities
b8 fl_ecs_iter(FlEcsCtx *p_ctx, size_t *p_iter, FlEntity *p_entity);

b8 fl_ecs_query(FlEcsCtx *p_ctx, size_t *p_iter, FlEntity *p_entity, FlComponent *components, size_t components_size);

FlEntity fl_ecs_entity_add(FlEcsCtx *p_ctx);

void* fl_ecs_get_entity_component_data(FlEcsCtx *p_ctx, FlEntity entity, FlComponent component);

b8 fl_ecs_entity_has_component(FlEcsCtx *p_ctx, FlEntity entity, FlComponent component);

#endif // _FL_ECS_H
