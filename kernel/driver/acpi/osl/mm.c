#include <acpi.h>
#include <assert.h>
#include <mm/virtual.h>
#include <stdio.h>
#include <stdlib.h>

void *acpi_os_map_memory(acpi_physical_address PhysicalAddress, acpi_size length) {
	size_t overhang = (PhysicalAddress & 0xFFF);
	length = ALIGN_UP((length + overhang), 0x1000);
	uintptr_t virt = mm_virtual_alloc(length >> 12);
	uintptr_t phys = PhysicalAddress - overhang;

	for(size_t i = 0; i < length; i += 0x1000) {
		mm_virtual_map(phys + i, virt + i, 1, PAGE_PRESENT | PAGE_WRITE | PAGE_NX);
	}

	return (void *) (virt + overhang);
}

void acpi_os_unmap_memory(void *where, acpi_size length) {
	uintptr_t addr = (uintptr_t) where & ~0xFFFUL;
	size_t overhang = ((uintptr_t) where & 0xFFF);
	size_t len = ALIGN_UP((length + overhang), 0x1000);
	assert(len == mm_virtual_free(addr));
}

acpi_status acpi_os_get_physical_address(void *LogicalAddress, acpi_physical_address *PhysicalAddress) {
	*PhysicalAddress = mm_virtual_get_phys((uintptr_t) LogicalAddress);

	return AE_OK;
}

void *acpi_os_allocate(acpi_size size) {
	return malloc(size);
}

void acpi_os_free(void *Memory) {
	free(Memory);
}

uint8_t acpi_os_readable(vy_unused void *Memory, vy_unused acpi_size Length) {
	panic("%s unimplemented", __func__);
}

uint8_t acpi_os_writable(vy_unused void *Memory, vy_unused acpi_size Length) {
	panic("%s unimplemented", __func__);
}
