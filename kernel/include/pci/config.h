#pragma once

#include <stdint.h>

typedef struct {
	uint64_t addr;
	uint64_t size;
} pci_bar_t;

uint32_t pci_config_read(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset);
void pci_config_write(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset, uint32_t value);
uint8_t pci_config_header_type(uint32_t bus, uint32_t slot, uint32_t function);
uint16_t pci_config_vendor(uint32_t bus, uint32_t slot, uint32_t function);
uint16_t pci_config_device_id(uint32_t bus, uint32_t slot, uint32_t function);
uint16_t pci_config_class(uint32_t bus, uint32_t slot, uint32_t function);
uint8_t pci_config_interface_id(uint32_t bus, uint32_t slot, uint32_t function);
uint8_t pci_config_secondary_bus(uint32_t bus, uint32_t slot, uint32_t function);
void pci_config_bar(uint32_t bus, uint32_t slot, uint32_t function, uint8_t bar, pci_bar_t *info);
void pci_config_set_busmaster(uint32_t bus, uint32_t slot, uint32_t function);
uint8_t pci_config_interrupt_pin(uint32_t bus, uint32_t slot, uint32_t function);
