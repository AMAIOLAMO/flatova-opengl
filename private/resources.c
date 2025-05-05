#include <resources.h>

#include <assert.h>
#include <string.h>

static int resource_cmp(const void *a, const void *b, void *udata) {
    (void) udata;
    const Resource *ra = a;
    const Resource *rb = b;

    return strcmp(ra->id, rb->id);
}

static uint64_t resource_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const Resource *res = item;
    // default hash
    return hashmap_sip(res->id, strlen(res->id), seed0, seed1);
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

b8 resources_has(Resources resources, const char *id) {
    const Resource *resource = hashmap_get(resources, &(Resource){ .id = id });

    return resource != NULL;
}

const Resource* resources_store(Resources resources, const Resource *p_resource) {
    assert(p_resource && "ERROR: Cannot store resource of NULL");
    assert(p_resource->p_raw && "ERROR: p_resource->p_raw is NULL");
    assert(resources_find(resources, p_resource->id) == NULL && "ERROR: Cannot store resource when it has the same id!");

    return hashmap_set(resources, p_resource);
}

void* resources_find(Resources resources, const char *id) {
    const Resource *resource = hashmap_get(resources, &(Resource){ .id = id });

    return resource ? resource->p_raw : NULL;
}
