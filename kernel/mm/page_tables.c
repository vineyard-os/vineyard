#include <cpu/cpu.h>
#include <cpu/cr.h>
#include <cpu/msr.h>
#include <driver/uart.h>
#include <driver/framebuffer.h>
#include <mm/page.h>
#include <mm/page_tables.h>
#include <mm/physical.h>
#include <mm/uefi.h>
#include <mm/virtual.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define STACK 0xFFFFFE0000000000

static uint64_t pml4[512] __attribute__((aligned(0x1000)));

static uint64_t pml3_kernel[512] __attribute__((aligned(0x1000)));
static uint64_t pml2_kernel[512] __attribute__((aligned(0x1000)));
static uint64_t pml1_kernel_0[512] __attribute__((aligned(0x1000)));
static uint64_t pml1_kernel_1[512] __attribute__((aligned(0x1000)));

static uint64_t pml3_stack[512] __attribute__((aligned(0x1000)));
static uint64_t pml2_stack[512] __attribute__((aligned(0x1000)));
static uint64_t pml1_stack[512] __attribute__((aligned(0x1000)));

static uint64_t pml3_pmm[512] __attribute__((aligned(0x1000)));
static uint64_t pml2_pmm[512] __attribute__((aligned(0x1000)));
static uint64_t pml1_pmm[512] __attribute__((aligned(0x1000)));

void mm_page_tables_setup(void) {
	pml4[(STACK >> 39) & 0x1FFUL] = mm_uefi_get_phys((uintptr_t) &pml3_stack) | PAGE_WRITE | PAGE_PRESENT;
	pml4[510] = mm_uefi_get_phys((uintptr_t) &pml4) | PAGE_WRITE | PAGE_PRESENT;
	pml4[511] = mm_uefi_get_phys((uintptr_t) &pml3_kernel) | PAGE_WRITE | PAGE_PRESENT;

	/* map the kernel stack */
	size_t stack_size = 33;

	size_t stack_pml3i = (STACK >> 30) & 0x1FFUL;
	size_t stack_pml2i = (STACK >> 21) & 0x1FFUL;

	uintptr_t stack_phy = mm_uefi_get_phys((uintptr_t) STACK);

	pml3_stack[stack_pml3i] = mm_uefi_get_phys((uintptr_t) &pml2_stack) | PAGE_WRITE | PAGE_PRESENT;
	pml2_stack[stack_pml2i] = mm_uefi_get_phys((uintptr_t) &pml1_stack) | PAGE_WRITE | PAGE_PRESENT;

	for(size_t i = 0; i < stack_size; i++) {
		pml1_stack[i] = (stack_phy + (i << PAGE_SHIFT)) | PAGE_PRESENT | PAGE_WRITE | PAGE_NX;
	}

	/* map the kernel */
	pml3_kernel[510] = mm_uefi_get_phys((uintptr_t) &pml2_kernel) | PAGE_WRITE | PAGE_PRESENT;
	pml2_kernel[0] = mm_uefi_get_phys((uintptr_t) &pml1_kernel_0) | PAGE_WRITE | PAGE_PRESENT;
	pml2_kernel[1] = mm_uefi_get_phys((uintptr_t) &pml1_kernel_1) | PAGE_WRITE | PAGE_PRESENT;

	/* TODO: in principle, we should dynamically allocate as many pml1 tables as we need; we still are in UEFI */
	for(size_t i = 0; i < 511; i++) {
		uintptr_t phys = mm_uefi_get_phys(KERNEL_START + (i << PAGE_SHIFT));

		if(!phys) {
			return;
		}

		pml1_kernel_0[i] = phys | PAGE_PRESENT | PAGE_WRITE;
	}

	for(size_t i = 0; i < 511; i++) {
		uintptr_t phys = mm_uefi_get_phys(KERNEL_START + 0x200000 + (i << PAGE_SHIFT));

		if(!phys) {
			return;
		}

		pml1_kernel_1[i] = phys | PAGE_PRESENT | PAGE_WRITE;
	}
}

void mm_page_tables_commit(void) {
	/* set up the pmm's mappings */
	size_t pml4i_pmm = (STACK_ADDR >> 39) & 0x1FF;
	size_t pml3i_pmm = (STACK_ADDR >> 30) & 0x1FF;
	size_t pml2i_pmm = (STACK_ADDR >> 21) & 0x1FF;
	size_t pml1i_pmm = (STACK_ADDR >> 12) & 0x1FF;

	pml4[pml4i_pmm] = mm_uefi_get_phys((uintptr_t) &pml3_pmm) | PAGE_PRESENT | PAGE_WRITE | PAGE_NX;
	pml3_pmm[pml3i_pmm] = mm_uefi_get_phys((uintptr_t) &pml2_pmm) | PAGE_PRESENT | PAGE_WRITE | PAGE_NX;
	pml2_pmm[pml2i_pmm] = mm_uefi_get_phys((uintptr_t) &pml1_pmm) | PAGE_PRESENT | PAGE_WRITE | PAGE_NX;
	pml1_pmm[pml1i_pmm] = mm_physical_stack_pml1[511];

	framebuffer_ready = false;

	msr_write(0xC0000080, msr_read(0xC0000080) | 0x800);
	cr4_write(cr4_read() | 0x10);
	cr3_write(mm_uefi_get_phys((uintptr_t) &pml4));

	msr_write(MSR_GS_BASE, (uint64_t) &bsp);
	msr_write(MSR_GS_KERNEL_BASE, (uint64_t) &bsp);

	struct vm_indices i;
	mm_virtual_indices(&i, STACK_ADDR);
	mm_physical_stack_pml1 = i.pml1;

	mm_virtual_map((uintptr_t) framebuffer_lfb, (uintptr_t) framebuffer_lfb, framebuffer_lfb_size >> 12, PAGE_PRESENT | PAGE_WRITE | PAGE_NX);

	framebuffer_ready = true;
}
