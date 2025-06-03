#pragma once
#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <stddef.h>

struct FlAnimation_t;

typedef void (*fl_animate_func_t)(const struct FlAnimation_t *p_anim, void *p_val, float t);

typedef struct FlAnimation_t {
    float start, duration;
    fl_animate_func_t func;
} FlAnimation;

// returns the end time of an animation
float fl_anim_end(const FlAnimation *p_anim);

// updates the given value based on a list of animations
void fl_anim_update(const FlAnimation *anims, size_t anims_size, void *p_val, float t);

#endif // _ANIMATION_H
