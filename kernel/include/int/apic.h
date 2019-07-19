#pragma once

#include <int/isr.h>

void apic_init(void);
void apic_enable(void);
void apic_ack(void);
void apic_timer_monotonic(uint32_t ms, isr_handler_t handler);