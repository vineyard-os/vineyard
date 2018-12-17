#include <boot/info.h>
#include <stdio.h>
#include <uefi.h>

static const char *memory_map_types[] __attribute__((unused)) = {
	[EfiReservedMemoryType] = "Reserved",
	[EfiLoaderCode] = "Loader Code",
	[EfiLoaderData] = "Loader Data",
	[EfiBootServicesCode] = "Boot Services Code",
	[EfiBootServicesData] = "Boot Services Data",
	[EfiRuntimeServicesCode] = "Runtime Services Code",
	[EfiRuntimeServicesData] = "Runtime Services Data",
	[EfiConventionalMemory] = "Conventional Memory",
	[EfiUnusableMemory] = "Unusable Memory",
	[EfiACPIReclaimMemory] = "ACPI Reclaimable Memory",
	[EfiACPIMemoryNVS] = "ACPI NVS",
	[EfiMemoryMappedIO] = "Memory Mapped I/O",
	[EfiMemoryMappedIOPortSpace] = "Memory Mapped I/O Port Space",
	[EfiPalCode] = "Pal Code",
	[EfiPersistentMemory] = "Persistent Memory",
};

efi_status efi_get_memory_map(void) {
	size_t size = 0;
	size_t descriptor_size;
	uint32_t descriptor_version;
	efi_status status;

	status = info.st->BootServices->GetMemoryMap(&size, info.efi_memory_map, &info.efi_memory_map_key, &descriptor_size, &descriptor_version);

	if(EFI_ERROR(status) && status != EFI_BUFFER_TOO_SMALL) {
		return status;
	}

    status = info.st->BootServices->AllocatePool(EfiBootServicesData, size, (void **) &info.efi_memory_map);

    if(EFI_ERROR(status)) {
        return status;
	}

    status = info.st->BootServices->GetMemoryMap(&size, info.efi_memory_map, &info.efi_memory_map_key, &descriptor_size, &descriptor_version);

	if(EFI_ERROR(status) && status != EFI_BUFFER_TOO_SMALL) {
    	return status;
	}

	info.efi_memory_map_entries = size / descriptor_size;
	info.efi_memory_map_descriptor_size = descriptor_size;

	return EFI_SUCCESS;
}
