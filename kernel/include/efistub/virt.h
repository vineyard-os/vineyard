#pragma once

#include <efi.h>

#define PAGE_PRESENT 0x01
#define PAGE_WRITE 0x02
#define PAGE_NX 0x8000000000000000

efi_status efi_virt_map(uintptr_t physical, uintptr_t virt, size_t pages, uintptr_t flags);
void efi_virt_wp_disable(void);
