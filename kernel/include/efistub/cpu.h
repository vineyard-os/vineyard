#pragma once

#include <stdint.h>

void efi_msr_write(uint32_t msr, uint64_t value);
uint64_t efi_msr_read(uint32_t msr);

void efi_cr0_write(uint64_t);
uint64_t efi_cr0_read(void);
uint64_t efi_cr3_read(void);
