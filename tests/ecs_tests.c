#include <assert.h>
#include <fl_ecs.h>
#include <utils.h>

typedef struct ExampleComponent_t {
    size_t value;
    float a, b;
} ExampleComponent;

int main(void) {
    FlEcsCtx ecs_ctx = fl_create_ecs_ctx(500, 32);
    assert(ecs_ctx.component_cap == 32);
    assert(ecs_ctx.entity_cap == 500);
    assert(ecs_ctx.entity_count == 0);

    FlComponent component_id = fl_ecs_add_component(&ecs_ctx, sizeof(ExampleComponent));
    assert(ecs_ctx.component_byte_sizes[component_id] == sizeof(ExampleComponent));

    FlEntity e1 = fl_ecs_entity_add(&ecs_ctx);
    assert(fl_ecs_entity_has_component(&ecs_ctx, e1, component_id) == false);
    assert(ecs_ctx.entity_count == 1);

    fl_ecs_entity_activate_component(&ecs_ctx, e1, component_id, false);
    assert(fl_ecs_entity_has_component(&ecs_ctx, e1, component_id) == false);

    fl_ecs_entity_activate_component(&ecs_ctx, e1, component_id, true);
    assert(fl_ecs_entity_has_component(&ecs_ctx, e1, component_id) == true);

    ExampleComponent *p_comp = fl_ecs_get_entity_component_data(&ecs_ctx, e1, component_id);
    p_comp->value = 200;
    p_comp->a = 40.01f;
    p_comp->b = -100.0f; 

    ExampleComponent *p_comp2 = fl_ecs_get_entity_component_data(&ecs_ctx, e1, component_id);
    assert(p_comp2->value == 200 && p_comp2->a == 40.01f && p_comp2->b == -100.0f);
    

    fl_ecs_ctx_free(&ecs_ctx);

    return 0;
}
