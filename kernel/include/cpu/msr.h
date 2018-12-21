#pragma once

#include <stdint.h>

#define MSR_GS_BASE			0xC0000101
#define MSR_GS_KERNEL_BASE	0xC0000102

void msr_write(uint32_t msr, uint64_t value);
uint64_t msr_read(uint32_t msr);
