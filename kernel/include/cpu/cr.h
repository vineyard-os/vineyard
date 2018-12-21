#pragma once

#include <stdint.h>

uint64_t cr0_read(void);
void cr0_write(uint64_t cr0);
uint64_t cr2_read(void);
uint64_t cr3_read(void);
void cr3_write(uint64_t cr3);
uint64_t cr4_read(void);
void cr4_write(uint64_t cr4);
