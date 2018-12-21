#pragma once

#include <cpu/gdt.h>
#include <stddef.h>

typedef struct cpu {
	struct cpu *self;
	gdt_entry_t gdt[GDT_ENTRIES];
	gdt_gdtr_t gdtr;
} cpu_t;

void cpu_bsp_init(void);
cpu_t *cpu_get(void);
