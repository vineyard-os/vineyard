#pragma once

#include <stdint.h>
#include <vy.h>

typedef struct {
	uint16_t limit;
	uint64_t base;
} vy_packed idtr_t;

void idt_init(void);
void idt_load(idtr_t *idtr);
