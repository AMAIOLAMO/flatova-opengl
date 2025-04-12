#pragma once
#ifndef _RESOURCES_H
#define _RESOURCES_H

#include <hashmap.h>

#include <utils.h>

#define RES_TYPE_SHADER   0x1
#define RES_TYPE_TEXTURE  0x2
#define RES_TYPE_FONT     0x3

typedef void (*res_free_func_t)(void *p_raw);

typedef struct Resource_t {
    const char *identifier;
    const uint32_t type;
    void *p_raw;

    res_free_func_t free;
} Resource;


typedef struct hashmap* Resources;

// creates a resources list, needed to use "resource_manager_free" to free the manager itself
Resources resources_create(void);

// deallocate the resources list
void resources_free(Resources resources);

// stores the resource location given the id, and the raw ptr of the resource.
// this does not automatically deallocate the resource.
const Resource* resources_store(Resources resources, const char *id, void* p_res);

// stores the resource location given the id, and the raw ptr of the resource.
// automatically deallocates the resource using the given free_func.
const Resource* resources_store_auto(Resources resources, const char *id, void* p_res, res_free_func_t free_func);

// finds the resource given the id, returns the raw ptr of the resource
void* resources_find(Resources resources, const char *id);

// iterates through the resources, and returns a resource ptr for each iteration
b8 resources_iter(Resources resources, size_t *p_iter, Resource **pp_resource);


#endif // _RESOURCES_H
