#pragma once

#include <vy.h>

typedef struct {
	uint64_t addr;
	uint16_t seg;
	uint8_t bus_start;
	uint8_t bus_end;
	uint32_t reserved;
} vy_packed pcie_config_t;

typedef struct {
	uint16_t seg;
	uint8_t bus_start;
	uint8_t bus_end;
	uintptr_t phys;
} pcie_segment_t;

void pci_express_init(void);
uint64_t pci_express_read(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset);