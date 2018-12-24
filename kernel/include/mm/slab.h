#pragma once

#include <util/list.h>
#include <stdint.h>

#define SLAB_VM_BASE 0xFFFFFEFFFFFF8000UL
#define SLAB_SIZE 0x4000UL

typedef struct {
	list_t slabs_full;
	list_t slabs_partial;
	list_t slabs_free;
	size_t object_size;
	size_t objects_per_slab;
	const char *name;
	node_t *next;
} slab_cache_t;

typedef struct {
	/* starting address of the first object within the slab */
	uintptr_t start;
	/* number of active objects in the slab */
	size_t inuse;
	/* array of free objects in the slab */
	size_t free[1];
} slab_t;

slab_cache_t *mm_cache_create_vm(size_t obj_size);
void *mm_slab_alloc(slab_cache_t *cache);
void mm_slab_free(slab_cache_t *cache, void *object);

extern slab_cache_t vm;
