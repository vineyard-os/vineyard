#include <boot/info.h>
#include <efi.h>
#include <efi/protocol/file.h>
#include <uefi.h>

efi_status main(efi_handle handle, efi_system_table *st, uintptr_t copy);

info_t info;

efi_status main(efi_handle handle, efi_system_table *st, uintptr_t copy __attribute__((unused))) {

	info.handle = handle;
	info.st = st;

	efi_init();

	printf("vineyard\n");

	for(;;);

	return EFI_SUCCESS;
}
