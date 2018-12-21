#pragma once

#include <stdint.h>
#include <vy.h>

#define GDT_ENTRIES 5

typedef struct {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t flags;
	uint8_t granularity;
	uint8_t base_high;
} vy_packed gdt_entry_t;

typedef struct {
	uint16_t limit;
	uint64_t base;
} vy_packed gdt_gdtr_t;

void gdt_init(void);
void gdt_gdtr(gdt_gdtr_t *gdt_gdtr);
