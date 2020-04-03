#include <cpu/cpu.h>
#include <cpu/cr.h>
#include <cpu/msr.h>
#include <driver/uart.h>
#include <driver/framebuffer.h>
#include <mm/page.h>
#include <mm/page_tables.h>
#include <mm/physical.h>
#include <mm/virtual.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define STACK 0xFFFFFE0000000000

void mm_page_tables_commit(void) {
	uintptr_t pmm_stack_addr = mm_physical_stack_pml1[511] & 0x000FFFFFFFFFF000;
	uintptr_t pmm_stack_flags = mm_physical_stack_pml1[511] & 0xFFF0000000000FFF;

	mm_virtual_map(pmm_stack_addr, STACK_ADDR, 1, pmm_stack_flags);

	msr_write(0xC0000080, msr_read(0xC0000080) | 0x800);
	cr4_write(cr4_read() | 0x10);

	msr_write(MSR_GS_BASE, (uint64_t) &bsp);
	msr_write(MSR_GS_KERNEL_BASE, (uint64_t) &bsp);

	struct vm_indices i;
	mm_virtual_indices(&i, STACK_ADDR);
	mm_physical_stack_pml1 = i.pml1;
}
