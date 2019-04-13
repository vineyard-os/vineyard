#pragma once

#include <cpu/gdt.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct cpu {
	struct cpu *self;
	gdt_entry_t gdt[GDT_ENTRIES];
	gdt_gdtr_t gdtr;
	bool apic_lint_nmi0;
	bool apic_lint_nmi1;
	uint8_t apic_id;
	uint8_t acpi_id;
} cpu_t;

void cpu_bsp_init(void);
cpu_t *cpu_get(void);

extern cpu_t bsp;
