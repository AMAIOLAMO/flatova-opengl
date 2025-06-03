#include <core/animation.h>
#include <assert.h>

float fl_anim_end(const FlAnimation *p_anim) {
    return p_anim->start + p_anim->duration;
}

void fl_anim_update(const FlAnimation *anims, size_t anims_size, void *p_val, float t) {
    assert(anims && "ERROR: animations cannot be NULL -> anims is NULL");

    for(size_t i = 0; i < anims_size; i++) {
        const FlAnimation *p_anim = &anims[i];

        assert(p_anim->func && "ERROR: animation function cannot be NULL -> anims[i]->func is NULL");

        if(t < p_anim->start && t <= fl_anim_end(p_anim))
            return;
        
        p_anim->func(p_anim, p_val, t);
    }
    // else

}
