#pragma once

#include <efi.h>
#include <stdio.h>

#define EFIERR(x) if(EFI_ERROR((x))) { printf("EFIERR %#lx @ %s:%d\n", (x), __FILE__, __LINE__); for(;;); }


void efi_init(void);
efi_status efi_print(const char *str);

efi_status efi_gop_get(efi_graphics_output_protocol **out);
efi_status efi_get_memory_map(void);
efi_status efi_get_image(efi_loaded_image_protocol **prot);
efi_status efi_get_cmdline(char **cmdline);
efi_status efi_get_rsdp(void);

#define EFI_OVMF_SIGNATURE "E\0D\0K\0 \0I\0I\0\0"
