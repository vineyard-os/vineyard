#include <pci/config.h>
#include <pci/walk.h>
#include <stdio.h>
#include <vy.h>

void pci_walk_device(uint8_t bus, uint8_t device, bool (*handler)(uint8_t bus, uint8_t device, uint8_t function)) {
	for(uint8_t function = 0; function < 8; function++) {
		if(pci_config_vendor(bus, device, function) == 0xFFFF) {
			continue;
		}

		handler(bus, device, function);

		uint32_t class = pci_config_class(bus, device, function);

		if(class == 0x0604) {
			uint8_t secondary = pci_config_secondary_bus(bus, device, function);

			if(secondary) {
				pci_walk_bus(secondary, handler);
			}
		}
	}
}

void pci_walk_bus(uint8_t bus, bool (*handler)(uint8_t bus, uint8_t device, uint8_t function)) {
	uint8_t device;

	for(device = 0; device < 32; device++) {
		if(pci_config_vendor(bus, device, 0) == 0xFFFF) {
			continue;
		}

		if(pci_config_header_type(bus, device, 0) & 0x80) {
			/* multi-function */
			pci_walk_device(bus, device, handler);
		} else {
			uint32_t class = pci_config_class(bus, device, 0);

			uint8_t secondary = pci_config_secondary_bus(bus, device, 0);

			if(!handler(bus, device, 0)) {
				return;
			}

			if(class == 0x0604) {
				if(secondary) {
					pci_walk_bus(secondary, handler);
				}
			}
		}
	}
}

void pci_walk_devices(bool (*handler)(uint8_t bus, uint8_t device, uint8_t function)) {
	if(pci_config_header_type(0, 0, 0) & 0x80) {
		/* multiple PCI host controllers */
		for(uint8_t function = 0; function < 8; function++) {
			if(pci_config_vendor(0, 0, function) != 0xFFFF) {
				continue;
			}

			pci_walk_bus(function, handler);
		}
	} else {
		/* one PCI host controller */
		pci_walk_bus(0, handler);
	}
}
