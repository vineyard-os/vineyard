#include <boot/info.h>
#include <string.h>
#include <uefi.h>

efi_status efi_get_rsdp() {
	efi_guid acpi_guid = ACPI_20_TABLE_GUID;

	efi_configuration_table *t = info.st->ConfigurationTable;

	for(size_t i = 0; i < info.st->NumberOfTableEntries && t; i++, t++) {
		if(!memcmp(&acpi_guid, &t->VendorGuid, 16)) {
			info.rsdp = t->VendorTable;
			return EFI_SUCCESS;
		}
	}

	return EFI_NOT_FOUND;
}
