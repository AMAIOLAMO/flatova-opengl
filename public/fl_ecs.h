#pragma once
#ifndef _FL_ECS_H
#define _FL_ECS_H

#include <utils.h>

typedef size_t FlEntityId;
typedef size_t FlComponent;

typedef b8 FlTableItem;
typedef FlTableItem* FlTable;

typedef struct FlEcsCtx_t {
    FlTable table;
    struct hashmap *id2idx_map;

    void **data_lists;

    size_t entity_count;

    size_t g_entity_id;
    size_t entity_cap;

    size_t g_component_id;
    size_t component_cap;

    size_t *component_byte_sizes;
} FlEcsCtx;

// creates the ecs given the maxiumum entity capacity and maximum component capacity
FlEcsCtx fl_create_ecs_ctx(size_t entity_cap, size_t component_cap);

// frees the ecs
void fl_ecs_ctx_free(FlEcsCtx *p_ctx);

// registers a new component into the ecs
FlComponent fl_ecs_add_component(FlEcsCtx *p_ctx, size_t component_bytes_size);

// activates the components for an entity
void fl_ecs_entity_activate_component(FlEcsCtx *p_ctx, FlEntityId entity, FlComponent component, b8 active);

// Iterates through all the entities
b8 fl_ecs_iter(FlEcsCtx *p_ctx, size_t *p_iter, FlEntityId *p_entity);

// tries to query entities given a list of components
b8 fl_ecs_query(FlEcsCtx *p_ctx, size_t *p_iter, FlEntityId *p_entity, FlComponent *components, size_t components_size);

FlEntityId fl_ecs_entity_add(FlEcsCtx *p_ctx);
void fl_ecs_entity_free(FlEcsCtx *p_ctx, FlEntityId entity);
b8 fl_ecs_entity_valid(FlEcsCtx *p_ctx, FlEntityId entity);

// gets the raw pointer of the entity's associated data
void* fl_ecs_get_entity_component_data(FlEcsCtx *p_ctx, FlEntityId entity, FlComponent component);

// returns whether or not this entity contains a component
b8 fl_ecs_entity_has_component(FlEcsCtx *p_ctx, FlEntityId entity, FlComponent component);

#endif // _FL_ECS_H
