#include <assert.h>
#include <stdio.h>
#include <mm/physical.h>
#include <mm/virtual.h>
#include <sys/mman.h>

int munmap(void *addr, size_t len) {
	if(!addr || ((uintptr_t) addr & 0x3FF) || !len) {
		return -1;
	}

	size_t pages = len >> 12;

	for(size_t i = 0; i < pages; i++) {
		uintptr_t phys = mm_virtual_get_phys((uintptr_t) addr + (i << 12));

		mm_virtual_map(0, (uintptr_t) addr + (i << 12), 1, 0);
		mm_physical_mark_free(phys);
	}

	size_t region = mm_virtual_free((uintptr_t) addr);

	assert(region == len);

	return 0;
}
