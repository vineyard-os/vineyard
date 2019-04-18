#pragma once

#include <efi.h>

#define EFIERR(x) if(EFI_ERROR((x))) { efi_printf((char *) "EFIERR %s @ %s:%d\n", efi_get_error(x), __FILE__, __LINE__); for(;;); }
#define efi_panic(msg, code) { st->ConOut->OutputString(st->ConOut, (char16_t *) (msg)); for(;;); return (code); }

extern efi_handle handle;
extern efi_system_table *st;

efi_status efi_print(const char *str);
efi_status efi_printn(const char *str, size_t n);
__attribute__((format(printf, 1, 2))) int efi_printf(const char *restrict format, ...);

efi_status efi_main(efi_handle image_handle, efi_system_table *systab);
void efi_loader_jump_to_kernel(efi_handle handle, efi_system_table *st, uintptr_t copy, uintptr_t entry);

const char *efi_get_error(efi_status status);
