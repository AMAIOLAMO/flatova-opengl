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
} FlEcsCtx;

FlEcsCtx fl_ecs_ctx_create(FlEntity entity_cap, FlComponent component_cap);

void fl_ecs_ctx_free(FlEcsCtx *p_ctx);

FlComponent fl_ecs_add_component(FlEcsCtx *p_ctx, size_t component_bytes_size);

void fl_ecs_entity_activate_component(FlEcsCtx *p_ctx, FlEntity entity, FlComponent component_id, b8 active);

b8 fl_ecs_query(FlEcsCtx *p_ctx, size_t *p_iter,
                FlComponent component_id, size_t component_byte_size, FlEntity *p_entity, void **p_component_data);

FlEntity fl_ecs_entity_add(FlEcsCtx *p_ctx);

void* fl_ecs_get_entity_component_data(FlEcsCtx *p_ctx, FlEntity entity,
                                       FlComponent component, size_t component_byte_size);


#endif // _FL_ECS_H
