#pragma once

#include <stdint.h>

#define ZONE_NORMAL 0
#define ZONE_DMA 1
#define ZONE_DMA32 2

#define ZONE_DMA_LIMIT 0xFFFFFF /* 16 MiB - 1 */
#define ZONE_DMA32_LIMIT 0xFFFFFFFF /* 4 GiB - 1 */

extern uint64_t *mm_physical_stack_pml1;

void mm_physical_mark_free(uint64_t addr);
uint64_t mm_physical_get(void);
void mm_physical_init(void);
