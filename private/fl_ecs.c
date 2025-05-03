#include "hashmap.h"
#include <assert.h>
#include <fl_ecs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct FlId2Idx_t {
    FlEntity id;
    size_t idx;
} FlId2Idx;

static int id2idx_cmp(const void *a, const void *b, void *udata) {
    (void) udata;
    const FlId2Idx *ra = a;
    const FlId2Idx *rb = b;

    return ra->id - rb->id;
}

static uint64_t id2idx_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const FlId2Idx *id2idx = item;
    // default hash
    return hashmap_sip(&id2idx->id, sizeof(id2idx->id), seed0, seed1);
}



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

FlEcsCtx fl_create_ecs_ctx(size_t entity_cap, size_t component_cap) {
    return (FlEcsCtx) {
        .table = fl_ecs_table_create(entity_cap, component_cap),
        .id2idx_map = hashmap_new(sizeof(FlId2Idx), 0, 0, 0,
                           id2idx_hash, id2idx_cmp, NULL, NULL),
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
    hashmap_free(p_ctx->id2idx_map);
    free(p_ctx->component_byte_sizes);
}

// grabs the associated id based on the entity given
static size_t fl_ecs_entity_get_associated_idx(FlEcsCtx *p_ctx, FlEntity entity_id) {
    const FlId2Idx *p_id2idx = hashmap_get(p_ctx->id2idx_map, &(FlId2Idx){ .id = entity_id });
    assert(p_id2idx != NULL);

    return p_id2idx->idx;
}

// sets the association between the entity id to its idx
static void fl_ecs_entity_associate(FlEcsCtx *p_ctx, FlEntity entity_id, size_t idx) {
    hashmap_set(p_ctx->id2idx_map, &(FlId2Idx){ .id = entity_id, .idx = idx });
}

// removes the association between the entity id to its idx
static void fl_ecs_entity_unassociate(FlEcsCtx *p_ctx, FlEntity entity_id) {
    hashmap_delete(p_ctx->id2idx_map, &(FlId2Idx){ .id = entity_id });
}

// COMPONENT ROW, ENTITY COLUMN
static inline size_t fl_ecs_get_comp_table_idx(FlEcsCtx *p_ctx, FlEntity entity_id, FlComponent component_id) {
    return component_id * p_ctx->entity_cap + fl_ecs_entity_get_associated_idx(p_ctx, entity_id);
}


FlComponent fl_ecs_add_component(FlEcsCtx *p_ctx, size_t component_bytes_size) {
    const FlComponent COMP_ID = p_ctx->g_component_id++;

    p_ctx->data_lists[COMP_ID]           = malloc(component_bytes_size * p_ctx->entity_cap);
    p_ctx->component_byte_sizes[COMP_ID] = component_bytes_size;

    return COMP_ID;
}

void fl_ecs_entity_activate_component(FlEcsCtx *p_ctx, FlEntity entity, FlComponent component, b8 active) {
    p_ctx->table[fl_ecs_get_comp_table_idx(p_ctx, entity, component)] = active;
}

b8 fl_ecs_iter(FlEcsCtx *p_ctx, size_t *p_iter, FlEntity *p_entity) {
    FlId2Idx *p_id2idx;

    while(hashmap_iter(p_ctx->id2idx_map, p_iter, (void**)&p_id2idx)) {
        *p_entity = p_id2idx->id;

        return true;
    }

    return false;
}

b8 fl_ecs_query(FlEcsCtx *p_ctx, size_t *p_iter, FlEntity *p_entity, FlComponent *components, size_t components_size) {
    // checking each and individual entity
    FlId2Idx *p_id2idx;

    while(hashmap_iter(p_ctx->id2idx_map, p_iter, (void**)&p_id2idx)) {
        b8 has_all_components = true;

        // check if the current component has all the required components
        for(size_t i_comp = 0; i_comp < components_size; i_comp++) {
            if(!p_ctx->table[fl_ecs_get_comp_table_idx(p_ctx, p_id2idx->id, components[i_comp])]) {
                has_all_components = false;
                break;
            }
        }

        if(has_all_components) {
            *p_entity = p_id2idx->id;
            return true;
        }
    }

    return false;
}

FlEntity fl_ecs_entity_add(FlEcsCtx *p_ctx) {
    assert(p_ctx->g_entity_id < p_ctx->entity_cap);

    FlEntity new_id = p_ctx->g_entity_id++;

    fl_ecs_entity_associate(p_ctx, new_id, p_ctx->entity_count);

    for(size_t i_comp = 0; i_comp < p_ctx->component_cap; i_comp++) {
        size_t tb_idx = fl_ecs_get_comp_table_idx(p_ctx, new_id, i_comp);
        p_ctx->table[tb_idx] = false;
    }

    p_ctx->entity_count += 1;
    return new_id;
}

// COMPONENT ROW, ENTITY COLUMN
size_t fl_ecs_entity_get_table_idx(FlEcsCtx *p_ctx, FlEntity entity) {
    size_t entity_idx = fl_ecs_entity_get_associated_idx(p_ctx, entity);
    
    return entity_idx * sizeof(FlTableItem);
}

void fl_ecs_entity_free(FlEcsCtx *p_ctx, FlEntity entity) {
    if(p_ctx->entity_count == 1) {
        fl_ecs_entity_unassociate(p_ctx, entity);
        p_ctx->entity_count -= 1;
        return;
    }
    // else

    size_t current_idx = fl_ecs_entity_get_associated_idx(p_ctx, entity);
    size_t last_idx = p_ctx->entity_count - 1;
    
    // grab last entity
    size_t iter = 0;
    FlId2Idx *p_id2idx;

    FlEntity last_entity = 0;

    while(hashmap_iter(p_ctx->id2idx_map, &iter, (void**)&p_id2idx)) {
        if(p_id2idx->idx == last_idx) {
            last_entity = p_id2idx->id;
            break;
        }
    }

    // unordered copy from last entity to the current
    for(size_t i_comp = 0; i_comp < p_ctx->component_cap; i_comp++) {
        // copy table
        b8 last_entity_comp_value = p_ctx->table[fl_ecs_get_comp_table_idx(p_ctx, last_entity, i_comp)];
        p_ctx->table[fl_ecs_get_comp_table_idx(p_ctx, entity, i_comp)] = last_entity_comp_value;

        // copy data
        if(last_entity_comp_value == false)
            continue;
        // else

        b8 *data_list = (b8*)p_ctx->data_lists[i_comp];
        assert(data_list);

        const size_t component_byte_size = p_ctx->component_byte_sizes[i_comp];

        memcpy(
            &data_list[component_byte_size * current_idx],
            &data_list[component_byte_size * last_idx],
            component_byte_size
        );
    }


    // remove from the associations
    fl_ecs_entity_unassociate(p_ctx, entity);

    // associate the last entity with the new index
    fl_ecs_entity_associate(p_ctx, last_entity, current_idx);

    p_ctx->entity_count -= 1;
}

void* fl_ecs_get_entity_component_data(FlEcsCtx *p_ctx, FlEntity entity_id, FlComponent component) {
    b8 *data_list = (b8*)p_ctx->data_lists[component];

    assert(data_list);

    const size_t component_byte_size = p_ctx->component_byte_sizes[component];
    return &data_list[component_byte_size * fl_ecs_entity_get_associated_idx(p_ctx, entity_id)];
}

b8 fl_ecs_entity_has_component(FlEcsCtx *p_ctx, FlEntity entity, FlComponent component) {
    return p_ctx->table[fl_ecs_get_comp_table_idx(p_ctx, entity, component)];
}


