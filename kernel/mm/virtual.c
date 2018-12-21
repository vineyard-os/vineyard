#include <mm/page.h>
#include <mm/physical.h>
#include <mm/tlb.h>
#include <mm/virtual.h>
#include <mm/page_tables.h>
#include <stdint.h>
#include <string.h>

#define PML1_OFFSET 0xFFFFFF0000000000
#define PML2_OFFSET 0xFFFFFF7F80000000
#define PML3_OFFSET 0xFFFFFF7FBFC00000
#define PML4_OFFSET 0xFFFFFF7FBFDFE000

#define PML1_SHIFT 12
#define PML2_SHIFT 21
#define PML3_SHIFT 30
#define PML4_SHIFT 39

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

static vy_status_t mm_virtual_indices(struct vm_indices *i, uintptr_t addr) {
	if(addr >= HIGHER_HALF) {
		addr -= HIGHER_HALF;
		i->pml4i = 256 + ((addr >> PML4_SHIFT) & 0x1FF);
	} else {
		i->pml4i = (addr >> PML4_SHIFT) & 0x1FF;
	}

	i->pml3i = (addr >> PML3_SHIFT) & 0x1FF;
	i->pml2i = (addr >> PML2_SHIFT) & 0x1FF;
	i->pml1i = (addr >> PML1_SHIFT) & 0x1FF;

	i->pml4 = (uint64_t *) PML4_OFFSET;
	i->pml3 = (uint64_t *) (PML3_OFFSET + (i->pml4i << PML1_SHIFT));
	i->pml2 = (uint64_t *) (PML2_OFFSET + (i->pml4i << PML2_SHIFT) + (i->pml3i << PML1_SHIFT));
	i->pml1 = (uint64_t *) (PML1_OFFSET + (i->pml4i << PML3_SHIFT) + (i->pml3i << PML2_SHIFT) + (i->pml2i << PML1_SHIFT));

	return VY_OK;
}

vy_status_t mm_virtual_map(uintptr_t phys, uintptr_t virt, size_t pages, uintptr_t flags) {
	struct vm_indices i;

	mm_virtual_indices(&i, virt);

	if(!(i.pml4[i.pml4i] & PAGE_PRESENT)) {
		uintptr_t new_table = mm_physical_get();
		i.pml4[i.pml4i] = new_table | PAGE_PRESENT | PAGE_WRITE;

		memset(i.pml3, 0, 0x1000);
	}


	if(!(i.pml3[i.pml3i] & PAGE_PRESENT)) {
		uintptr_t new_table = mm_physical_get();
		i.pml3[i.pml3i] = new_table | PAGE_PRESENT | PAGE_WRITE;

		memset(i.pml2, 0, 0x1000);
	}


	if(!(i.pml2[i.pml2i] & PAGE_PRESENT)) {
		uintptr_t new_table = mm_physical_get();
		i.pml2[i.pml2i] = new_table | PAGE_PRESENT | PAGE_WRITE;

		memset(i.pml1, 0, 0x1000);
	}

	for(size_t c = 0; c < pages; c++) {
		i.pml1[i.pml1i + c] = (phys + (c << PAGE_SHIFT)) | flags;
		mm_tlb_invlpg(virt + (c << PAGE_SHIFT));

		if(i.pml1i + c > 511) {
			mm_virtual_map(phys + (c << PAGE_SHIFT), virt + (c << PAGE_SHIFT), pages - c, flags);
			break;
		}
	}

	return VY_OK;
}

extern void *KERNEL_SIZE;

vy_status_t mm_virtual_init(void) {
	mm_page_tables_setup();

	return VY_OK;
}
