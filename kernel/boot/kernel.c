#include <acpi.h>
#include <boot/info.h>
#include <cpu/cpu.h>
#include <efi.h>
#include <efi/protocol/file.h>
#include <mm/physical.h>
#include <mm/virtual.h>
#include <uefi.h>
#include <vy.h>

efi_status main(efi_handle h, efi_system_table *st, uintptr_t copy);

info_t info;

void _init(void);

efi_status main(efi_handle h, efi_system_table *st, uintptr_t vy_unused copy) {
	_init();

	info.handle = h;
	info.st = st;

	efi_init();
	cpu_bsp_init();
	mm_physical_init();
	mm_virtual_init();
	acpi_init();

	printf("vineyard\n");

	for(;;);

	return EFI_SUCCESS;
}
