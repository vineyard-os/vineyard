#pragma once

#include <stdbool.h>
#include <stdint.h>

void pci_walk_devices(bool (*handler)(uint8_t bus, uint8_t device, uint8_t function));
void pci_walk_bus(uint8_t bus, bool (*handler)(uint8_t bus, uint8_t device, uint8_t function));
void pci_walk_device(uint8_t bus, uint8_t device, bool (*handler)(uint8_t bus, uint8_t device, uint8_t function));
