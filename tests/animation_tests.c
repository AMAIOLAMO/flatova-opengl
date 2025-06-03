#include <assert.h>
#include <core/animation.h>

void linear_func(const FlAnimation *p_anim, void *p_val, float t) {
    float *p_float = p_val;

    *p_float = t * p_anim->duration;
}

void test_construct(void) {
    FlAnimation animation = {
        .start = 1.0f,
        .duration = 2.0f,
        .func = linear_func
    };

    float value = 1.0f;
    fl_anim_update(&animation, 1, &value, 0.0f);
    assert(value == 1.0f);

    fl_anim_update(&animation, 1, &value, 1.0f);
    assert(value == animation.duration);
}

void test_multi_anim(void) {
    FlAnimation a = {
        .start = 0.0f,
        .duration = 1.0f,
        .func = linear_func
    };

    FlAnimation b = {
        .start = 1.0f,
        .duration = 2.0f,
        .func = linear_func
    };

}

int main(void) {
    test_construct();

    return 0;
}


