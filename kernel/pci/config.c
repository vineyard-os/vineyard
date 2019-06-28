#include <assert.h>
#include <boot/panic.h>
#include <cpu/ports.h>
#include <pci/config.h>
#include <stdio.h>

#define _BAR(x) 0x10 + (uint32_t) ((x) << 2)

uint32_t pci_config_read(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset) {
	uint32_t addr = (bus << 16) | (slot << 11) | (func << 8) | (1U << 31) | offset;

	if((offset & 3)) {
		printf("invalid offset %#x for device %02x:%02x.%02x\n", offset, bus, slot, func);
		assert(!(offset & 3));
	}

	outl(0xCF8, addr);

	return inl(0xCFC);
}

void pci_config_write(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset, uint32_t value) {
	uint32_t addr = (bus << 16) | (slot << 11) | (func << 8) | (1U << 31) | offset;

	if((offset & 3)) {
		printf("invalid offset %#x for device %02x:%02x.%02x\n", offset, bus, slot, func);
		assert(!(offset & 3));
	}

	outl(0xCF8, addr);
	outl(0xCFC, value);
}

uint8_t pci_config_header_type(uint32_t bus, uint32_t slot, uint32_t function) {
	return (pci_config_read(bus, slot, function, 0x0C) >> 16) & 0xFF;
}

uint16_t pci_config_vendor(uint32_t bus, uint32_t slot, uint32_t function) {
	return pci_config_read(bus, slot, function, 0x00) & 0xFFFF;
}

uint16_t pci_config_device_id(uint32_t bus, uint32_t slot, uint32_t function) {
	return (pci_config_read(bus, slot, function, 0x00) >> 16) & 0xFFFF;
}

uint16_t pci_config_class(uint32_t bus, uint32_t slot, uint32_t function) {
	return (pci_config_read(bus, slot, function, 0x08) >> 16) & 0xFFFF;
}

uint8_t pci_config_interface_id(uint32_t bus, uint32_t slot, uint32_t function) {
	return (pci_config_read(bus, slot, function, 0x08) >> 8) & 0xFF;
}

uint8_t pci_config_secondary_bus(uint32_t bus, uint32_t slot, uint32_t function) {
	return (pci_config_read(bus, slot, function, 0x18) >> 8) & 0xFF;
}

void pci_config_bar(uint32_t bus, uint32_t slot, uint32_t function, uint8_t bar, pci_bar_t *info) {
	info->addr = pci_config_read(bus, slot, function, _BAR(bar));

	if(info->addr & 1) {
		/* I/O space */
		panic("I/O space\n");
	} else {
		if(info->addr & 4) {
			/* 64 bits */
			info->addr &= 0xFFFFFFFFFFFFFFF0;
			info->addr |= (uint64_t) pci_config_read(bus, slot, function, _BAR(bar + 1)) << 32;
			pci_config_write(bus, slot, function, _BAR(bar), 0xFFFFFFFF);
			pci_config_write(bus, slot, function, _BAR(bar + 1), 0xFFFFFFFF);

			info->size = pci_config_read(bus, slot, function, _BAR(bar));
			info->size |= (uint64_t) pci_config_read(bus, slot, function, _BAR(bar + 1)) << 32;

			pci_config_write(bus, slot, function, _BAR(bar), info->addr & 0xFFFFFFFF);
			pci_config_write(bus, slot, function, _BAR(bar + 1), (info->addr >> 32) & 0xFFFFFFFF);

			info->size = (~(info->size & 0xFFFFFFFFFFFFFFF0) + 1);
		} else {
			/* 32 bits */
			pci_config_write(bus, slot, function, _BAR(bar), 0xFFFFFFFF);
			uint32_t size = pci_config_read(bus, slot, function, _BAR(bar));
			pci_config_write(bus, slot, function, _BAR(bar), info->addr & 0xFFFFFFFF);
			info->size = (~(size & 0xFFFFFFF0) + 1) & 0xFFFFFFFF;
			info->addr &= 0xFFFFFFFFFFFFFFF0;
		}
	}
}

void pci_config_set_busmaster(uint32_t bus, uint32_t slot, uint32_t function) {
	uint32_t reg = pci_config_read(bus, slot, function, 0x04);
	reg |= (1U << 2);
	pci_config_write(bus, slot, function, 0x04, reg);
}

uint8_t pci_config_interrupt_pin(uint32_t bus, uint32_t slot, uint32_t function) {
	return (pci_config_read(bus, slot, function, 0x3C) >> 8) & 0xFF;
}
