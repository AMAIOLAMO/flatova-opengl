#include <assert.h>
#include <stdio.h>

#include <utils.h>
#include <core/fl_ecs.h>

typedef struct ExampleComponent_t {
    size_t value;
    float a, b;
} ExampleComponent;

typedef struct ExampleComponent2_t {
    b8 value;
    float a;
} ExampleComponent2;

int main(void) {
    FlEcsCtx ecs_ctx = fl_create_ecs_ctx(100, 32);
    assert(ecs_ctx.component_cap == 32);
    assert(ecs_ctx.entity_cap == 100);
    assert(ecs_ctx.entity_count == 0);

    FlComponent test_component_id = fl_ecs_add_component(&ecs_ctx, sizeof(ExampleComponent));
    assert(ecs_ctx.component_byte_sizes[test_component_id] == sizeof(ExampleComponent));

    FlComponent test_component2_id = fl_ecs_add_component(&ecs_ctx, sizeof(ExampleComponent2));
    assert(ecs_ctx.component_byte_sizes[test_component2_id] == sizeof(ExampleComponent2));

    FlEntityId e0 = fl_ecs_entity_add(&ecs_ctx);
    assert(fl_ecs_entity_has_component(&ecs_ctx, e0, test_component_id) == false);
    assert(ecs_ctx.entity_count == 1);
    assert(fl_ecs_entity_valid(&ecs_ctx, e0) == true);

    fl_ecs_entity_free(&ecs_ctx, e0);
    assert(ecs_ctx.entity_count == 0);
    assert(fl_ecs_entity_valid(&ecs_ctx, e0) == false);

    FlEntityId e1 = fl_ecs_entity_add(&ecs_ctx);
    assert(fl_ecs_entity_has_component(&ecs_ctx, e1, test_component_id) == false);
    assert(ecs_ctx.entity_count == 1);

    FlEntityId e2 = fl_ecs_entity_add(&ecs_ctx);
    assert(fl_ecs_entity_has_component(&ecs_ctx, e2, test_component_id) == false);
    assert(ecs_ctx.entity_count == 2);

    size_t iter = 0;
    FlEntityId q_entity;
    FlComponent comps[] = {
        test_component_id
    };
    size_t count = 0;
    while(fl_ecs_query(&ecs_ctx, &iter, &q_entity, comps, arr_size(comps)))
        count += 1;

    assert(count == 0);

    fl_ecs_entity_activate_component(&ecs_ctx, e1, test_component_id, false);
    assert(fl_ecs_entity_has_component(&ecs_ctx, e1, test_component_id) == false);

    fl_ecs_entity_activate_component(&ecs_ctx, e1, test_component_id, true);
    assert(fl_ecs_entity_has_component(&ecs_ctx, e1, test_component_id) == true);

    fl_ecs_entity_activate_component(&ecs_ctx, e2, test_component_id, true);
    assert(fl_ecs_entity_has_component(&ecs_ctx, e2, test_component_id) == true);

    iter = 0;
    count = 0;
    while(fl_ecs_query(&ecs_ctx, &iter, &q_entity, comps, arr_size(comps)))
        count += 1;

    assert(count == 2);

    ExampleComponent *p_comp = fl_ecs_get_entity_component_data(&ecs_ctx, e2, test_component_id);
    p_comp->value = 200;
    p_comp->a = 40.01f;
    p_comp->b = -100.0f; 

    p_comp = fl_ecs_get_entity_component_data(&ecs_ctx, e2, test_component_id);
    assert(p_comp->value == 200 && p_comp->a == 40.01f && p_comp->b == -100.0f);

    assert(fl_ecs_entity_valid(&ecs_ctx, e1) == true);

    fl_ecs_entity_free(&ecs_ctx, e1);
    assert(ecs_ctx.entity_count == 1);

    assert(fl_ecs_entity_valid(&ecs_ctx, e1) == false);

    FlEntityId e3 = fl_ecs_entity_add(&ecs_ctx);
    assert(fl_ecs_entity_has_component(&ecs_ctx, e3, test_component_id) == false);
    assert(fl_ecs_entity_has_component(&ecs_ctx, e3, test_component2_id) == false);
    assert(ecs_ctx.entity_count == 2);

    fl_ecs_entity_activate_component(&ecs_ctx, e3, test_component2_id, true);
    assert(fl_ecs_entity_has_component(&ecs_ctx, e3, test_component_id) == false);
    ExampleComponent2 *p_comp2 = fl_ecs_get_entity_component_data(&ecs_ctx, e3, test_component2_id);
    p_comp2->a = 200;
    p_comp2->value = false;

    iter = 0;
    count = 0;
    while(fl_ecs_iter(&ecs_ctx, &iter, &q_entity))
        count += 1;

    if(count != 2) {
        fprintf(stderr, "Error: %zu\n", count);
    }

    assert(count == 2);

    p_comp = fl_ecs_get_entity_component_data(&ecs_ctx, e2, test_component_id);
    assert(p_comp->value == 200 && p_comp->a == 40.01f && p_comp->b == -100.0f);

    fl_ecs_entity_free(&ecs_ctx, e2);
    assert(ecs_ctx.entity_count == 1);


    fl_ecs_ctx_free(&ecs_ctx);

    return 0;
}
