#pragma once
#ifndef _FL_CORE_RESOURCES_H
#define _FL_CORE_RESOURCES_H

#include <hashmap.h>

#include <core/types.h>

typedef enum ResType_t {
    FL_RES_SHADER_PROG,
    FL_RES_VERTICES,
    FL_RES_MODEL,
    FL_RES_TEXTURE,
    FL_RES_FONT,
    FL_RES_OTHER
} ResType;

// Represents a resource automatic freeing function, this is used to automatically free resources allocated
typedef void (*res_free_func_t)(void *p_raw);

typedef struct Resource_t {
    const char *id;
    const uint16_t type;
    void *p_raw;

    res_free_func_t free;
} Resource;


typedef struct hashmap* Resources;

// creates a resources list, needed to use "resource_manager_free" to free the manager itself
Resources resources_create(void);

// deallocate the resources list
void resources_free(Resources resources);

// returns the number of elements loaded in resources
size_t resources_size(Resources resources);

// stores the resource location given the id, type, raw resource, and automatic free function(optional)
// this does not automatically deallocate the resource.
const Resource* resources_store(Resources resources, const Resource *p_resource);

// stores the resource location given the id, and the raw ptr of the resource.
// automatically deallocates the resource using the given free_func.
// const Resource* resources_store_auto(Resources resources, const char *id, void* p_res, res_free_func_t free_func);

// finds the resource given the id, returns the raw ptr of the resource if found, returns NULL otherwise
void* resources_find(Resources resources, const char *id);

// iterates through the resources, and returns a resource ptr for each iteration
b8 resources_iter(Resources resources, size_t *p_iter, Resource **pp_resource);


#endif // _FL_CORE_RESOURCES_H
