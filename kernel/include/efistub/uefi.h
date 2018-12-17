#pragma once

#include <efi.h>

efi_status efi_print(const char *str);
__attribute__((format(printf, 1, 2))) int efi_printf(const char *restrict format, ...);

extern efi_system_table *st;

efi_status efi_main(efi_handle image_handle, efi_system_table *systab);
void efi_loader_jump_to_kernel(efi_handle handle, efi_system_table *st, uintptr_t copy, uintptr_t entry);
