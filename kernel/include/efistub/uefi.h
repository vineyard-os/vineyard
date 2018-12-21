#pragma once

#include <efi.h>

#define EFIERR(x) if(EFI_ERROR((x))) { efi_printf((char *) "EFIERR %#lx @ %s:%d\n", (x), __FILE__, __LINE__); for(;;); }
#define efi_panic(msg, code) { st->ConOut->OutputString(st->ConOut, (char16_t *) (msg)); st->BootServices->Stall(1000 * 1000 * 10); return (code); }

extern efi_handle handle;
extern efi_system_table *st;

efi_status efi_print(const char *str);
__attribute__((format(printf, 1, 2))) int efi_printf(const char *restrict format, ...);

efi_status efi_main(efi_handle image_handle, efi_system_table *systab);
void efi_loader_jump_to_kernel(efi_handle handle, efi_system_table *st, uintptr_t copy, uintptr_t entry);
