#include <acpi.h>
#include <boot/info.h>
#include <cpu/cpu.h>
#include <driver/nvme.h>
#include <debug/trace.h>
#include <efi.h>
#include <fs/gpt.h>
#include <mm/physical.h>
#include <mm/virtual.h>
#include <pci/config.h>
#include <pci/express.h>
#include <pci/walk.h>
#include <uefi.h>
#include <vy.h>

efi_status main(info_t *passed_info);

info_t info;

void _init(void);

static bool callback(uint8_t bus, uint8_t slot, uint8_t function) {
	uint16_t classes = pci_express_read(0, bus, slot, function, 0x0A) & 0xFFFF;

#ifdef CONFIG_PCI_DEBUG
	printf("PCI %02x:%02x.%02x %#x\n", bus, slot, function, pci_config_class(bus, slot, function));
#endif

	if(classes == 0x0108) {
#ifdef CONFIG_NVME_DEBUG
		printf("NVMe: controller at %02x:%02x.%02x\n", bus, slot, function);
#endif
		nvme_t *controller = nvme_init(bus, slot, function);
		gpt_init(&controller->namespace_list[0]);
	}

	return true;
}

efi_status main(info_t *passed_info) {
	_init();

	info = *passed_info;

	efi_init();
	cpu_bsp_init();
	mm_physical_init();
	mm_virtual_init();
	debug_init(info.copy, info.copy_size);
	acpi_init();

	pci_express_init();
	pci_walk_devices(&callback);

	printf("vineyard\n");

	for(;;);

	return EFI_SUCCESS;
}
