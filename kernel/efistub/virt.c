#include <efi.h>
#include <stdint.h>
#include <string.h>
#include <efistub/cpu.h>
#include <efistub/uefi.h>
#include <efistub/virt.h>

uintptr_t *pml4 = NULL;

efi_status efi_virt_map(uintptr_t physical, uintptr_t virt, size_t pages, uintptr_t flags) {
	if(!pml4) {
		pml4 = (uintptr_t *) (uintptr_t) efi_cr3_read();
	}

	size_t pml4i = (virt >> 39) & 0x1FF;
	size_t pml3i = (virt >> 30) & 0x1FF;
	size_t pml2i = (virt >> 21) & 0x1FF;
	size_t pml1i = (virt >> 12) & 0x1FF;

	uint64_t *pml3 = NULL;
	uint64_t *pml2 = NULL;
	uint64_t *pml1 = NULL;

	if(!pml4[pml4i]) {
		st->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, 1, (uintptr_t *) &pml3);
		memset(pml3, 0, 0x1000);

		pml4[pml4i] = (uintptr_t) pml3 | PAGE_PRESENT | PAGE_WRITE;
	} else {
		pml3 = (uintptr_t *) (pml4[pml4i] & ~0xFFFUL);
	}

	if(!pml3[pml3i]) {
		st->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, 1, (uintptr_t *) &pml2);
		memset(pml2, 0, 0x1000);

		pml3[pml3i] = (uintptr_t) pml2 | PAGE_PRESENT | PAGE_WRITE;
	} else {
		pml2 = (uintptr_t *) (pml3[pml3i] & ~0xFFFUL);
	}

	if(!pml2[pml2i]) {
		st->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, 1, (uintptr_t *) &pml1);
		memset(pml1, 0, 0x1000);

		pml2[pml2i] = (uintptr_t) pml1 | PAGE_PRESENT | PAGE_WRITE;
	} else {
		pml1 = (uintptr_t *) (pml2[pml2i] & ~0xFFFUL);
	}

	for(size_t i = 0; i < pages; i++) {
		pml1[pml1i + i] = (physical + (i << 12)) | flags;
		asm volatile ("invlpg (%0)" : : "r" (virt + (i << 12)) : "memory");

		if(pml1i + i > 511) {
			efi_virt_map(physical + (i << 12), virt + (i << 12), pages - i, flags);
		}
	}

	return EFI_SUCCESS;
}

void efi_virt_wp_disable(void) {
	uint64_t cr0 = efi_cr0_read();
	cr0 &= ~(1UL << 16);
	efi_cr0_write(cr0);
}
