#pragma once

#include <stdint.h>
#include <vy.h>

#define LVT_TIMER_INT 0xFD
#define LVT_ERROR_INT 0xFE

typedef struct {
	uint16_t limit;
	uint64_t base;
} vy_packed idtr_t;

void idt_init(void);
void idt_load(idtr_t *idtr);
