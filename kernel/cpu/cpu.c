#include <cpu/cpu.h>
#include <cpu/msr.h>
#include <int/idt.h>
#include <string.h>
#include <stdio.h>

static cpu_t bsp;

void cpu_bsp_init(void) {
	memset(&bsp, 0, sizeof(bsp));

	bsp.self = &bsp;

	msr_write(MSR_GS_BASE, (uint64_t) &bsp);
	msr_write(MSR_GS_KERNEL_BASE, (uint64_t) &bsp);

	gdt_init();
	idt_init();
}
