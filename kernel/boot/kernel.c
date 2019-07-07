#include <acpi.h>
#include <boot/info.h>
#include <cpu/cpu.h>
#include <driver/nvme.h>
#include <efi.h>
#include <fs/gpt.h>
#include <mm/physical.h>
#include <mm/virtual.h>
#include <pci/config.h>
#include <pci/walk.h>
#include <uefi.h>
#include <vy.h>

efi_status main(efi_handle h, efi_system_table *st, uintptr_t copy);

info_t info;

void _init(void);

static bool callback(uint8_t bus, uint8_t slot, uint8_t function) {
	uint16_t classes = pci_config_class(bus, slot, function);

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

efi_status main(efi_handle h, efi_system_table *st, uintptr_t vy_unused copy) {
	_init();

	info.handle = h;
	info.st = st;

	efi_init();
	cpu_bsp_init();
	mm_physical_init();
	mm_virtual_init();
	acpi_init();

	pci_walk_devices(&callback);

	printf("vineyard\n");

	for(;;);

	return EFI_SUCCESS;
}
