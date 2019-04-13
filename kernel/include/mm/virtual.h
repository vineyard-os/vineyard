#pragma once

#include <vy.h>
#include <stddef.h>

#define PAGE_PRESENT 0x01UL
#define PAGE_WRITE 0x02UL
#define PAGE_BIG 0x80UL
#define PAGE_NX 0x8000000000000000UL

#define HIGHER_HALF 0xFFFF800000000000
#define PAGE_TABLES 0xFFFFFF0000000000
#define KERNEL_START 0xFFFFFFFF80000000

#define VM_FREE 1UL
#define VM_USED 2UL

#define PML1_OFFSET 0xFFFFFF0000000000
#define PML2_OFFSET 0xFFFFFF7F80000000
#define PML3_OFFSET 0xFFFFFF7FBFC00000
#define PML4_OFFSET 0xFFFFFF7FBFDFE000

#define PML1_SHIFT 12
#define PML2_SHIFT 21
#define PML3_SHIFT 30
#define PML4_SHIFT 39

// #define PHY(x) ((uintptr_t) (x) - KERNEL_START + 0x100000)
/* align an address to the nearest page boundary higher or equal to the address */
#define PAGE_UP(x) ((0xFFF & (x)) ? (((x) + 0x1000) & ~0xFFFUL) : (x))

struct vm_indices {
	size_t pml4i;
	size_t pml3i;
	size_t pml2i;
	size_t pml1i;
	uint64_t *pml4;
	uint64_t *pml3;
	uint64_t *pml2;
	uint64_t *pml1;
};

vy_status_t mm_virtual_init(void);

vy_status_t mm_virtual_indices(struct vm_indices *i, uintptr_t addr);
vy_status_t mm_virtual_map(uintptr_t phys, uintptr_t virt, size_t pages, uintptr_t flags);

void mm_virtual_range_set(uintptr_t addr, size_t len, uintptr_t flags);

uintptr_t mm_virtual_alloc(size_t pages);
size_t mm_virtual_free(uintptr_t addr);

uintptr_t mm_virtual_get_phys(uintptr_t virt);
uintptr_t mm_virtual_get_flags(uintptr_t virt);

void mm_virtual_dump(void);
