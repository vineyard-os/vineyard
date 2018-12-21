#include <cpu/cpu.h>
#include <cpu/gdt.h>
#include <mm/virtual.h>
#include <stdio.h>
#include <string.h>

static void gdt_set(uint8_t sel, uint8_t flags, uint8_t gran) {
	cpu_t *cpu = cpu_get();
	gdt_entry_t *e = &cpu->gdt[sel];

	e->flags = flags;
	e->granularity = (uint8_t) ((gran << 4) | 0x0F);
	e->limit_low = 0xFFFF;
}

void gdt_init(void) {
	cpu_t *cpu = cpu_get();

	memset(&cpu->gdt, 0, sizeof(cpu->gdt));

	/* kernel code */
	gdt_set(1, 0x98, 2);
	/* kernel data */
	gdt_set(2, 0x92, 0);
	/* user code */
	gdt_set(3, 0xF8, 0);
	/* user data */
	gdt_set(4, 0xF2, 2);

	cpu->gdtr.base = (uintptr_t) cpu->gdt;
	cpu->gdtr.limit = sizeof(*cpu->gdt) * GDT_ENTRIES - 1;

	gdt_gdtr(&cpu->gdtr);
}
