#include <driver/nvme.h>
#include <pci/config.h>

void nvme_init(uint8_t bus, uint8_t slot, uint8_t function) {
	pci_config_set_busmaster(bus, slot, function);
}
