#pragma once

#include <stddef.h>
#include <stdint.h>

void debug_init(uintptr_t elf, size_t elf_size);
void debug_trace(void);
uint64_t debug_trace_rbp(void);
