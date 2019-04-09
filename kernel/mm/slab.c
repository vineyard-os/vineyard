#include <assert.h>
#include <boot/panic.h>
#include <mm/slab.h>
#include <mm/physical.h>
#include <mm/virtual.h>
#include <stdio.h>
#include <string.h>
#include <vy.h>

slab_cache_t vm;
static node_t vm_node;

static slab_t *mm_slab_create_initial(slab_cache_t *cache) {
	slab_t *slab = (slab_t *) SLAB_VM_BASE;

	mm_virtual_map(mm_physical_get(), SLAB_VM_BASE, 1, PAGE_PRESENT | PAGE_WRITE | PAGE_NX);
	mm_virtual_map(mm_physical_get(), SLAB_VM_BASE + 0x1000, 1, PAGE_PRESENT | PAGE_WRITE | PAGE_NX);
	mm_virtual_map(mm_physical_get(), SLAB_VM_BASE + 0x2000, 1, PAGE_PRESENT | PAGE_WRITE | PAGE_NX);
	mm_virtual_map(mm_physical_get(), SLAB_VM_BASE + 0x3000, 1, PAGE_PRESENT | PAGE_WRITE | PAGE_NX);

	slab->start = SLAB_VM_BASE + SLAB_SIZE - (cache->objects_per_slab * cache->object_size);
	slab->inuse = 0;

	for(size_t i = 0; i < cache->objects_per_slab; i++) {
		slab->free[i] = i;
	}

	memset((void *) slab->start, 0, cache->objects_per_slab * cache->object_size);

	return slab;
}

static slab_t *mm_slab_create(slab_cache_t *cache __attribute__((unused))) {
	/* get a memory range from the vmm */

	/* map the memory */

	/* set up the slab */

	panic("mm_slab_create unimplemented");

	return NULL;
}

slab_cache_t *mm_cache_create_vm(size_t obj_size) {
	size_t header_size = sizeof(slab_t) - sizeof(size_t);
	size_t objects_per_slab = (SLAB_SIZE - header_size) / (obj_size + sizeof(size_t));
	header_size += sizeof(size_t) * objects_per_slab;

	memset(&vm, 0, sizeof(vm));
	vm.name = "vm";
	vm.object_size = obj_size;
	vm.objects_per_slab = objects_per_slab;

	vm_node.data = mm_slab_create_initial(&vm);

	list_append(&vm.slabs_free, &vm_node);

	return &vm;
}

static slab_t *mm_slab_get(slab_cache_t *cache) {
	if(cache->slabs_partial.tail) {
		return (slab_t *) cache->slabs_partial.tail->data;
	} else if(cache->slabs_free.tail) {
		return (slab_t *) cache->slabs_free.tail->data;
	} else {
		panic("no slab space left");
	}
}

void *mm_slab_alloc(slab_cache_t *cache) {
	slab_t *slab = mm_slab_get(cache);

	if(!slab) {
		panic("no slab found");
	}

	uintptr_t addr = slab->start;
	size_t index = cache->objects_per_slab - slab->inuse - 1;
	addr += cache->object_size * slab->free[index];

	/* the list this slab is in */
	list_t *queue = NULL;
	/* the list this slab would move up into */
	list_t *queue_up = NULL;

	if(slab->inuse == 0) {
		queue = &cache->slabs_free;
		queue_up = &cache->slabs_partial;
	} else {
		queue = &cache->slabs_partial;
		queue_up = &cache->slabs_full;
	}

	slab->free[index] = 0;
	slab->inuse++;

	if(cache->objects_per_slab - slab->inuse <= 1) {
		list_append(queue_up, queue->tail);
		list_remove(queue, queue->tail);

		mm_slab_create(cache);
	}

	memset((void *) addr, 0, cache->object_size);

	return (void *) addr;
}

no_warn_gcc(-Wunused-but-set-variable)
void mm_slab_free(slab_cache_t *cache, void *object) {
	slab_t *slab = (void *) ((uintptr_t) object & ~(SLAB_SIZE - 1));
	// printf("slab = %#018lx, slab->inuse = %zu, cache = %#018lx\n", (uintptr_t) slab, slab->inuse, (uintptr_t) cache);

	uintptr_t offset = (uintptr_t) object - slab->start;
	size_t object_index = offset / cache->object_size;
	size_t free_objects = cache->objects_per_slab - slab->inuse;

	/* the list this slab is in */
	list_t *queue = NULL;
	/* the list this slab would move down to */
	list_t *queue_down = NULL;

	if(slab->inuse == 1) {
		queue = &cache->slabs_partial;
		queue_down = &cache->slabs_free;
	} else {
		queue = &cache->slabs_full;
		queue_down = &cache->slabs_partial;
	}

	slab->inuse--;
	slab->free[free_objects] = object_index;
}
