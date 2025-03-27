#include <resources.h>
#include <string.h>

static int resource_cmp(const void *a, const void *b, void *udata) {
    (void) udata;
    const Resource *ra = a;
    const Resource *rb = b;

    return strcmp(ra->identifier, rb->identifier);
}

static uint64_t resource_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const Resource *res = item;
    // default hash
    return hashmap_sip(res->identifier, strlen(res->identifier), seed0, seed1);
}



Resources resources_create(void) {
    return hashmap_new(sizeof(Resource), 0, 0, 0,
                       resource_hash, resource_cmp, NULL, NULL);
}

b8 resources_iter(Resources resources, size_t *p_iter, Resource **pp_resource) {
    return hashmap_iter(resources, p_iter, (void**)pp_resource);
}

void resources_free(Resources resources) {
    size_t iter = 0;
    Resource *p_res = NULL;

    while(resources_iter(resources, &iter, &p_res)) {
        if(p_res->free)
            p_res->free(p_res->p_raw);
    }

    hashmap_free(resources);
}

const Resource* resources_store(Resources resources, const char *id, void* p_res) {
    return resources_store_auto(resources, id, p_res, NULL);
}

const Resource* resources_store_auto(Resources resources, const char *id, void* p_res, res_free_func_t free_func) {
    return hashmap_set(resources, &(Resource){ .identifier = id, .p_raw = p_res, .free = free_func });
}

const void* resources_find(Resources resources, const char *id) {
    const Resource *resource = hashmap_get(resources, &(Resource){ .identifier = id });

    return resource ? resource->p_raw : NULL;
}
