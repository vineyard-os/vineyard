#include <cpu/cr.h>
#include <mm/uefi.h>
#include <mm/virtual.h>
#include <stdio.h>

uintptr_t mm_uefi_get_phys(uintptr_t virt) {
    struct vm_indices i;

    if(virt >= HIGHER_HALF) {
		virt -= HIGHER_HALF;
		i.pml4i = 256 + ((virt >> PML4_SHIFT) & 0x1FF);
	} else {
		i.pml4i = (virt >> PML4_SHIFT) & 0x1FF;
	}

	i.pml3i = (virt >> PML3_SHIFT) & 0x1FF;
	i.pml2i = (virt >> PML2_SHIFT) & 0x1FF;
	i.pml1i = (virt >> PML1_SHIFT) & 0x1FF;

    uintptr_t cr3 = cr3_read();

	i.pml4 = (uint64_t *) cr3;
	i.pml3 = (uint64_t *) (i.pml4[i.pml4i] & ~0xFFFUL);
	i.pml2 = (uint64_t *) (i.pml3[i.pml3i] & ~0xFFFUL);
	i.pml1 = (uint64_t *) (i.pml2[i.pml2i] & ~0xFFFUL);

    if(i.pml4[i.pml4i] & PAGE_PRESENT && i.pml3[i.pml3i] & PAGE_PRESENT && i.pml2[i.pml2i] & PAGE_PRESENT && i.pml1[i.pml1i] & PAGE_PRESENT) {
		return (i.pml1[i.pml1i] & 0x000FFFFFFFFFF000);
	}

    return 0;
}
