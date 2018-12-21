#pragma once

#include <vy.h>
#include <stddef.h>

#define PAGE_PRESENT 0x01UL
#define PAGE_WRITE 0x02UL
#define PAGE_BIG 0x80UL
#define PAGE_NX 0x8000000000000000UL

#define HIGHER_HALF 0xFFFF800000000000
#define PAGE_TABLES 0xFFFFFF0000000000
#define KERNEL_START 0XFFFFFFFF80000000

#define VM_FREE 1
#define VM_USED 2

#define PHY(x) ((uintptr_t) (x) - KERNEL_START + 0x100000)
/* align an address to the nearest page boundary higher or equal to the address */
#define PAGE_UP(x) ((0xFFF & (x)) ? (((x) + 0x1000) & ~0xFFFUL) : (x))

vy_status_t mm_virtual_init(void);
vy_status_t mm_virtual_map(uintptr_t phys, uintptr_t virt, size_t pages, uintptr_t flags);
