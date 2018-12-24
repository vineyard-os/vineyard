#include <assert.h>
#include <mm/physical.h>
#include <mm/virtual.h>
#include <stdio.h>
#include <sys/mman.h>

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	assert(addr == NULL);
	assert(fd == -1);
	assert(flags & MAP_ANONYMOUS && flags & MAP_PRIVATE);
	assert(off == 0);

	uintptr_t f = 0;
	size_t pages = len >> 12;

	if(prot & PROT_READ) {
		f |= PAGE_PRESENT;
	}

	if(prot & PROT_WRITE) {
		f |= PAGE_WRITE;
	}

	if(!(prot & PROT_EXEC)) {
		f |= PAGE_NX;
	}

	if(prot & PROT_NONE) {
		f = 0;
	}

	uintptr_t virt = mm_virtual_alloc(pages);

	for(size_t i = 0; i < pages; i++) {
		uintptr_t phys = mm_physical_get();

		mm_virtual_map(phys, virt + (i << 12), 1, f);
	}

	return (void *) virt;
}
