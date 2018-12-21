#include <cpu/cr.h>
#include <int/isr.h>
#include <stdio.h>

static const char *exception_type[] = {
	[0x0] = "Divide-by-zero Error",
	[0x1] = "Debug",
	[0x2] = "Non-maskable Interrupt",
	[0x3] = "Breakpoint",
	[0x4] = "Overflow",
	[0x5] = "Bound Range Exceeded",
	[0x6] = "Invalid Opcode",
	[0x7] = "Device Not Available",
	[0x8] = "Double Fault",
	[0x9] = "Coprocessor Segment Overrun",
	[0xA] = "Invalid TSS",
	[0xB] = "Segment Not Present",
	[0xC] = "Stack-Segment Fault",
	[0xD] = "General Protection Fault",
	[0xE] = "Page Fault",
	[0x10] = "x87 Floating-Point Exception",
	[0x11] = "Alignment Check",
	[0x12] = "Machine Check",
	[0x13] = "SIMD Floating-Point Exception",
	[0x14] = "Virtualization Exception",
	[0x1E] = "Security Exception",
};

void isr_dispatch(cpu_state_t *state) {
	printf("%s (num = %lu, error = %#010lx) @ %#018lx\n", exception_type[state->id], state->id, state->error, state->rip);

	if(state->id == 13 || state->id == 14) {
		printf("cr0 = %#018lx, cr2 = %#018lx, cr3 = %#018lx, cr4 = %#018lx\n", cr0_read(), cr2_read(), cr3_read(), cr4_read());

		for(;;);
	}
}
